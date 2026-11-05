#pragma once

#include <stdint.h>

enum camkeypad_link {
    // Nothing heard for CAMKEYPAD_STALE_AFTER_MS, or the phone disconnected.
    CAMKEYPAD_LINK_NONE,
    CAMKEYPAD_LINK_OK,
    // The phone speaks a protocol version this firmware does not know.
    CAMKEYPAD_LINK_BAD_VERSION,
};

struct camkeypad_view {
    enum camkeypad_link link;
    uint8_t version;
    uint8_t state;
    // Projected to now: counts on locally between heartbeats while recording.
    uint32_t elapsed_ms;
    uint8_t mode;
    uint8_t phone_battery;
};

// Safe to call from any thread.
void camkeypad_status_view(struct camkeypad_view *out);
