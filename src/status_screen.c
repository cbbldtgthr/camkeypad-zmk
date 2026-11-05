#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <lvgl.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/kernel.h>

#include <zmk/activity.h>
#include <zmk/display/status_screen.h>
#include <zmk/keymap.h>

#include <camkeypad_protocol.h>

#include "camkeypad_status.h"

// The screen is a terminal: crisp monospace bitmap fonts, as small as they stay readable. The
// title and rule use 8px, 16 columns, and fill the yellow band of a blue/yellow SSD1306. The body
// sits below it in the small font, 25 columns by 6 rows.
#define BAND_HEIGHT 16
#define RULE "----------------"
#define REFRESH_MS 100
#define BLINK_MS 500
// Must match SERVICE in config/camkeypad.keymap.
#define SERVICE_LAYER 1

static const struct device *display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

LV_FONT_DECLARE(camkeypad_font_small);

static lv_obj_t *header_label;
static lv_obj_t *body_label;
static bool blanked;

static void set_text(lv_obj_t *label, const char *text) {
    if (strcmp(lv_label_get_text(label), text) != 0) {
        lv_label_set_text(label, text);
    }
}

static void format_elapsed(char *buf, size_t size, uint32_t ms) {
    uint32_t s = ms / 1000;

    if (s >= 3600) {
        snprintf(buf, size, "%u:%02u:%02u", s / 3600, (s / 60) % 60, s % 60);
    } else {
        snprintf(buf, size, "%02u:%02u", s / 60, s % 60);
    }
}

// Stay lit while recording or paused; otherwise follow ZMK's idle timeout.
static void update_blanking(const struct camkeypad_view *view) {
    bool busy = view->link == CAMKEYPAD_LINK_OK && (view->state == CAMKEYPAD_STATE_RECORDING ||
                                                     view->state == CAMKEYPAD_STATE_PAUSED);
    bool blank = !busy && zmk_activity_get_state() != ZMK_ACTIVITY_ACTIVE;

    if (blank != blanked) {
        blanked = blank;
        if (blank) {
            display_blanking_on(display);
        } else {
            display_blanking_off(display);
        }
    }
}

static void refresh(lv_timer_t *timer) {
    struct camkeypad_view view;
    char title[13] = "";
    char battery[8] = "";
    char elapsed[16];
    char body[64] = "";
    char header[48];

    camkeypad_status_view(&view);
    update_blanking(&view);

    if (view.link == CAMKEYPAD_LINK_OK && view.phone_battery <= 100) {
        snprintf(battery, sizeof(battery), "%u%%", view.phone_battery);
    }

    switch (view.link) {
    case CAMKEYPAD_LINK_NONE:
        strcpy(title, "NO LINK");
        break;
    case CAMKEYPAD_LINK_BAD_VERSION:
        snprintf(title, sizeof(title), "PROTO v%u?", view.version);
        break;
    case CAMKEYPAD_LINK_OK:
        strcpy(title, view.mode == CAMKEYPAD_MODE_VIDEO ? "VIDEO" : "PHOTO");
        format_elapsed(elapsed, sizeof(elapsed), view.elapsed_ms);
        switch (view.state) {
        case CAMKEYPAD_STATE_IDLE:
            strcpy(body, "state  idle");
            break;
        case CAMKEYPAD_STATE_RECORDING:
            snprintf(body, sizeof(body), "state  REC\ntime   %s", elapsed);
            break;
        case CAMKEYPAD_STATE_PAUSED:
            // Blink the frozen time so paused never reads as recording.
            snprintf(body, sizeof(body), "state  PAUSED\ntime   %s",
                     (k_uptime_get() / BLINK_MS) % 2 ? "" : elapsed);
            break;
        default:
            snprintf(body, sizeof(body), "state  %u?", view.state);
            break;
        }
        break;
    }

    if (zmk_keymap_layer_active(SERVICE_LAYER)) {
        strcpy(title, "SERVICE");
        strcpy(body, "start  enter bootloader\npause  hold 1s to unpair");
    }

    snprintf(header, sizeof(header), "%-12s%4s\n" RULE, title, battery);
    set_text(header_label, header);
    set_text(body_label, body);
}

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    header_label = lv_label_create(screen);
    lv_label_set_text(header_label, "");
    lv_obj_set_style_text_font(header_label, &lv_font_unscii_8, LV_PART_MAIN);
    // The font's line height is 9, which would push the rule out of the band.
    lv_obj_set_style_text_line_space(header_label, -1, LV_PART_MAIN);
    lv_obj_align(header_label, LV_ALIGN_TOP_LEFT, 0, 0);

    body_label = lv_label_create(screen);
    lv_label_set_text(body_label, "");
    lv_obj_set_style_text_font(body_label, &camkeypad_font_small, LV_PART_MAIN);
    lv_obj_align(body_label, LV_ALIGN_TOP_LEFT, 0, BAND_HEIGHT);

    lv_timer_create(refresh, REFRESH_MS, NULL);
    refresh(NULL);
    return screen;
}
