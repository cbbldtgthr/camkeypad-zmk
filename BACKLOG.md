# Backlog

## ZMK firmware

- `zmk-config` for nice!nano v2, built by GitHub Actions; `zmk-usb-logging` snippet for
  debug builds.
- Keymap: two keys, `C_VOL_UP` and `C_VOL_DN`.
- Module: the status GATT service, using the generated protocol header.
- Module: custom OLED status screen showing REC / PAUSED / IDLE with mm:ss, counted
  locally between heartbeats; NO LINK when nothing arrives for `stale_after_ms` or the
  link drops; `PROTO vN?` for an unknown protocol version.
- Stop ZMK deep sleep and display blanking from cutting in mid-recording.
- Pick the OLED module and wiring.
