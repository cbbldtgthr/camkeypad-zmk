// camkeypad coexistence test: an HID keyboard plus the camkeypad status GATT service on
// one nice!nano. Proves the phone lets an app write GATT to the pad while Android's HID
// host uses it as a keyboard. Logs over USB serial; type u / d to send volume up / down.

#include <bluefruit.h>

#include "camkeypad_protocol.h"

static const uint8_t service_uuid[16] = CAMKEYPAD_SERVICE_UUID_LE;
static const uint8_t status_char_uuid[16] = CAMKEYPAD_STATUS_CHAR_UUID_LE;

BLEDis bledis;
BLEHidAdafruit blehid;
BLEService status_service(service_uuid);
BLECharacteristic status_char(status_char_uuid);

static uint32_t last_status_ms;

static const char *state_name(uint8_t state) {
    switch (state) {
    case CAMKEYPAD_STATE_IDLE: return "IDLE";
    case CAMKEYPAD_STATE_RECORDING: return "RECORDING";
    case CAMKEYPAD_STATE_PAUSED: return "PAUSED";
    default: return "?";
    }
}

static void on_status_write(uint16_t conn_hdl, BLECharacteristic *chr, uint8_t *data, uint16_t len) {
    (void)chr;
    uint32_t now = millis();
    Serial.printf("[%lu ms, +%lu] write conn=%u len=%u:", now, now - last_status_ms, conn_hdl, len);
    for (uint16_t i = 0; i < len; i++) {
        Serial.printf(" %02x", data[i]);
    }
    Serial.println();
    last_status_ms = now;

    struct camkeypad_status status;
    if (camkeypad_status_decode(data, len, &status) != 0) {
        Serial.printf("  too short, want %d bytes\n", CAMKEYPAD_STATUS_LEN);
        return;
    }
    if (status.version != CAMKEYPAD_PROTOCOL_VERSION) {
        Serial.printf("  PROTO v%u? (pad speaks v%d)\n", status.version, CAMKEYPAD_PROTOCOL_VERSION);
        return;
    }
    uint32_t secs = status.elapsed_ms / 1000;
    Serial.printf("  %s %02lu:%02lu\n", state_name(status.state), secs / 60, secs % 60);
}

static void on_connect(uint16_t conn_hdl) {
    char name[32] = {0};
    Bluefruit.Connection(conn_hdl)->getPeerName(name, sizeof(name));
    Serial.printf("connected conn=%u peer=%s\n", conn_hdl, name);
}

static void on_disconnect(uint16_t conn_hdl, uint8_t reason) {
    Serial.printf("disconnected conn=%u reason=0x%02x\n", conn_hdl, reason);
}

static void send_consumer_key(uint16_t usage, const char *label) {
    if (!Bluefruit.connected()) {
        Serial.println("not connected, key not sent");
        return;
    }
    blehid.consumerKeyPress(usage);
    delay(10);
    blehid.consumerKeyRelease();
    Serial.printf("sent %s\n", label);
}

static void start_advertising() {
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
    Bluefruit.Advertising.addService(blehid);
    Bluefruit.Advertising.addName();
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244); // units of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(30);   // seconds in fast mode
    Bluefruit.Advertising.start(0);             // advertise forever
}

void setup() {
    Serial.begin(115200);
    // Don't block without a monitor attached, but give one a moment to open.
    for (uint32_t start = millis(); !Serial && millis() - start < 3000;) {
        delay(10);
    }
    Serial.println("camkeypad arduino_test");

    Bluefruit.begin();
    Bluefruit.setTxPower(4);
    Bluefruit.setName("camkeypad-test");
    Bluefruit.Periph.setConnectCallback(on_connect);
    Bluefruit.Periph.setDisconnectCallback(on_disconnect);

    bledis.setManufacturer("camkeypad");
    bledis.setModel("arduino_test");
    bledis.begin();

    blehid.begin();

    status_service.begin();
    status_char.setProperties(CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP);
    status_char.setPermission(SECMODE_NO_ACCESS, SECMODE_OPEN);
    status_char.setMaxLen(20); // room for fields appended by later protocol versions
    status_char.setWriteCallback(on_status_write);
    status_char.begin();

    start_advertising();
    Serial.println("advertising; pair 'camkeypad-test' from Android Bluetooth settings");
}

void loop() {
    while (Serial.available()) {
        switch (Serial.read()) {
        case 'u': send_consumer_key(HID_USAGE_CONSUMER_VOLUME_INCREMENT, "volume up"); break;
        case 'd': send_consumer_key(HID_USAGE_CONSUMER_VOLUME_DECREMENT, "volume down"); break;
        default: break;
        }
    }
    delay(10);
}
