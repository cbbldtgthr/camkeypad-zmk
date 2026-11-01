# Backlog

## Arduino coexistence test

Prove the phone lets an app talk GATT to the pad while Android's HID host uses it as a
keyboard. Adafruit nRF52 core on a bare nice!nano (Feather nRF52840 Express board
definition), USB serial for logs, no other hardware.

- Advertise as an HID keyboard; typing `u` / `d` in the serial monitor sends
  consumer volume up / down.
- Expose the camkeypad status service from the generated protocol header; log every
  write, decoded, over serial.
- Pass criteria: writes from nRF Connect arrive, volume keys still trigger Open Camera
  while the GATT client is connected, and still after it disconnects.
- Driven through a Justfile using `arduino-cli`, so no Arduino IDE is needed.

## Open Camera fork: push recording state

- New remote type next to Kraken in `remotecontrol/`; on services discovered, find the
  status characteristic instead of subscribing to notifications.
- Write a status on every video start, stop, pause and resume, including stops Open
  Camera makes on its own (max size or duration, storage full, errors).
- Heartbeat every `heartbeat_ms` while connected.
- One GATT write in flight at a time.
- The device picker may need to list bonded devices, since a pad already connected as
  HID may not advertise; skip the 10 s LE scan when connecting to it.

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
