# Backlog

## ZMK firmware

- Verify on the pad: the display stays lit through a recording longer than
  `CONFIG_ZMK_IDLE_TIMEOUT`, NO LINK on disconnect, and `PROTO vN?` for an unknown
  protocol version.
- Show the LiPo charge level on the OLED, so a flat pad doesn't catch you out mid-shoot.

## Exposure encoder (firmware and Open Camera fork)

- Add a second encoder for exposure, with its click locking and unlocking auto exposure.
  Open Camera has no key for either, so the fork needs key cases calling `changeExposure()`
  and `clickedExposureLock()`. Send `F10`, `F11` and `F12`: Android's key map leaves F13 and
  up commented out through Android 15. Check on the pad that the lock works while recording,
  and whether exposure can still change while locked.
