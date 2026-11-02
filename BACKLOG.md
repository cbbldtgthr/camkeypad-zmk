# Backlog

## ZMK firmware

- Verify on the pad: the display stays lit through a recording longer than
  `CONFIG_ZMK_IDLE_TIMEOUT`, NO LINK on disconnect, and `PROTO vN?` for an unknown
  protocol version.
- Show the LiPo charge level on the OLED, so a flat pad doesn't catch you out mid-shoot.

## Rotary encoder (firmware and Open Camera fork)

- Add a rotary encoder with a push switch for camera controls: turning changes zoom or
  another control such as exposure, and clicking locks or unlocks auto exposure. Open
  Camera already zooms on plain HID `+`/`-`; it has no dedicated key for exposure or for
  the auto exposure lock, so those need a change in the fork. Still to decide: how the
  pad chooses what the encoder controls, and which pins it uses.
