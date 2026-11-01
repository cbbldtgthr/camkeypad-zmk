#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <lvgl.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/kernel.h>

#include <zmk/activity.h>
#include <zmk/display/status_screen.h>

#include <camkeypad_protocol.h>

#include "camkeypad_status.h"

// The top 16 rows of a blue/yellow SSD1306 are the yellow band.
#define BAND_HEIGHT 16
#define REFRESH_MS 100
#define BLINK_MS 500

static const struct device *display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

static lv_obj_t *band_label;
static lv_obj_t *main_label;
static bool blanked;

static void set_text(lv_obj_t *label, const char *text, const lv_font_t *font) {
    if (strcmp(lv_label_get_text(label), text) != 0) {
        lv_label_set_text(label, text);
    }
    if (lv_obj_get_style_text_font(label, LV_PART_MAIN) != font) {
        lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
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
    char band[24];
    char main[24];
    const lv_font_t *main_font = &lv_font_montserrat_28;

    camkeypad_status_view(&view);
    update_blanking(&view);

    switch (view.link) {
    case CAMKEYPAD_LINK_NONE:
        band[0] = '\0';
        strcpy(main, "NO LINK");
        main_font = &lv_font_montserrat_16;
        break;
    case CAMKEYPAD_LINK_BAD_VERSION:
        band[0] = '\0';
        snprintf(main, sizeof(main), "PROTO v%u?", view.version);
        main_font = &lv_font_montserrat_16;
        break;
    case CAMKEYPAD_LINK_OK:
        format_elapsed(main, sizeof(main), view.elapsed_ms);
        switch (view.state) {
        case CAMKEYPAD_STATE_IDLE:
            strcpy(band, "IDLE");
            strcpy(main, "--:--");
            break;
        case CAMKEYPAD_STATE_RECORDING:
            strcpy(band, "REC");
            break;
        case CAMKEYPAD_STATE_PAUSED:
            strcpy(band, LV_SYMBOL_PAUSE " PAUSED");
            // Blink the frozen time so paused never reads as recording.
            if ((k_uptime_get() / BLINK_MS) % 2) {
                main[0] = '\0';
            }
            break;
        default:
            snprintf(band, sizeof(band), "STATE %u?", view.state);
            main[0] = '\0';
            break;
        }
        break;
    }

    set_text(band_label, band, &lv_font_montserrat_14);
    set_text(main_label, main, main_font);
}

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    band_label = lv_label_create(screen);
    lv_label_set_text(band_label, "");
    lv_obj_align(band_label, LV_ALIGN_TOP_LEFT, 0, 0);

    main_label = lv_label_create(screen);
    lv_label_set_text(main_label, "");
    lv_obj_align(main_label, LV_ALIGN_CENTER, 0, BAND_HEIGHT / 2);

    lv_timer_create(refresh, REFRESH_MS, NULL);
    refresh(NULL);
    return screen;
}
