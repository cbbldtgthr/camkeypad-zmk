#include <stdbool.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/l2cap.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <camkeypad_protocol.h>

#include "camkeypad_status.h"

LOG_MODULE_REGISTER(camkeypad, CONFIG_ZMK_LOG_LEVEL);

BUILD_ASSERT(BT_L2CAP_RX_MTU >= CAMKEYPAD_ATT_MTU,
             "raise CONFIG_BT_BUF_ACL_RX_SIZE to fit the protocol's att_mtu");

#define SERVICE_UUID BT_UUID_DECLARE_128(CAMKEYPAD_SERVICE_UUID_ENCODE)
#define STATUS_CHAR_UUID BT_UUID_DECLARE_128(CAMKEYPAD_STATUS_CHAR_UUID_ENCODE)

static struct k_spinlock lock;
static bool have_status;
static struct camkeypad_status last;
static int64_t received_at;
// The phone that sent the last status; only its disconnect drops the link.
static bt_addr_le_t sender;

static ssize_t write_status(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                            const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {
    const uint8_t *bytes = buf;
    struct camkeypad_status status = {0};

    if (offset != 0) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
    if (len < 1) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }
    // An unknown version may have a different layout, so only its version byte is kept.
    if (bytes[0] != CAMKEYPAD_PROTOCOL_VERSION) {
        status.version = bytes[0];
    } else if (camkeypad_status_decode(bytes, len, &status) < 0) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    k_spinlock_key_t key = k_spin_lock(&lock);
    have_status = true;
    last = status;
    received_at = k_uptime_get();
    bt_addr_le_copy(&sender, bt_conn_get_dst(conn));
    k_spin_unlock(&lock, key);

    LOG_DBG("status v%u state %u elapsed %u ms mode %u battery %u focus %u %u cm exposure %d lock %u rate %u project %s",
            status.version, status.state, status.elapsed_ms, status.mode, status.phone_battery,
            status.focus, status.focus_distance_cm, status.exposure_tenths,
            status.exposure_locked, status.capture_rate_tenths, status.project);
    return len;
}

BT_GATT_SERVICE_DEFINE(camkeypad_svc, BT_GATT_PRIMARY_SERVICE(SERVICE_UUID),
                       BT_GATT_CHARACTERISTIC(STATUS_CHAR_UUID,
                                              BT_GATT_CHRC_WRITE |
                                                  BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                                              BT_GATT_PERM_WRITE_ENCRYPT, NULL, write_status,
                                              NULL), );

static void disconnected(struct bt_conn *conn, uint8_t reason) {
    k_spinlock_key_t key = k_spin_lock(&lock);
    if (have_status && bt_addr_le_eq(&sender, bt_conn_get_dst(conn))) {
        have_status = false;
    }
    k_spin_unlock(&lock, key);
}

BT_CONN_CB_DEFINE(camkeypad_conn_cb) = {
    .disconnected = disconnected,
};

void camkeypad_status_view(struct camkeypad_view *out) {
    k_spinlock_key_t key = k_spin_lock(&lock);
    int64_t age = k_uptime_get() - received_at;

    *out = (struct camkeypad_view){.link = CAMKEYPAD_LINK_NONE};
    if (have_status && age <= CAMKEYPAD_STALE_AFTER_MS) {
        out->version = last.version;
        out->state = last.state;
        out->elapsed_ms = last.elapsed_ms;
        out->mode = last.mode;
        out->phone_battery = last.phone_battery;
        out->focus = last.focus;
        out->focus_distance_cm = last.focus_distance_cm;
        out->exposure_tenths = last.exposure_tenths;
        out->exposure_locked = last.exposure_locked;
        out->capture_rate_tenths = last.capture_rate_tenths;
        memcpy(out->project, last.project, sizeof(out->project));
        if (last.version != CAMKEYPAD_PROTOCOL_VERSION) {
            out->link = CAMKEYPAD_LINK_BAD_VERSION;
        } else {
            out->link = CAMKEYPAD_LINK_OK;
            if (last.state == CAMKEYPAD_STATE_RECORDING) {
                out->elapsed_ms += (uint32_t)age;
            }
        }
    }
    k_spin_unlock(&lock, key);
}
