# Bring-up learnings

What flashing this firmware onto real hardware taught us, and none of it is visible in
the code. For current behaviour read the code; for why it is that way read the commits.

## Reset the pad

Flashing firmware keeps the pad's saved settings, so a reflash alone will not clear them.
Reset when the pad won't pair, stops appearing in the phone's scan, advertises an old
name, or moves to a new phone. Run it first on any reused board.

1. Double-tap reset so the `NICENANO` drive appears.
2. Run `just fw-reset`, or `just fw-reset build/local/<name>.uf2` to finish on a local build.
3. When it asks, wait 5 seconds and double-tap reset again.
4. On the phone, forget `camkeypad` if it is listed, then pair it again.
5. In the Open Camera build, select `camkeypad` again in the remote settings.

This erases the pad's pairings and saved Bluetooth name. Without an argument it finishes on
the CI firmware in `build/firmware`, which `just fw-fetch` refreshes, so run that first if
that copy is old.

## Symptoms and causes

| What you see | Cause | Fix | How sure |
|---|---|---|---|
| Green CI, but after flashing: no USB, no Bluetooth, OLED full of random pixels | A custom screen gets the default 2048-byte LVGL pool. ZMK only raises it to 4096 for its built-in screen. | `CONFIG_LV_Z_MEM_POOL_SIZE=4096` | Going to 4096 fixed it. No crash log was captured. |
| Random pixels on the OLED | The display was switched on but nothing was ever drawn. Wiring and address are fine; look at the firmware. | | Inferred, and fits the row above |
| Pad advertises under an old name ("Seagull") | Flashing a UF2 does not erase ZMK's settings area, and a saved Bluetooth name overrides `CONFIG_ZMK_KEYBOARD_NAME`. | `just fw-reset` | Fixed by the reset |
| Phone says "pairing…", then nothing | Old pairing records on the pad, most likely. Forgetting the pad on the phone does not clear the pad's side. | Same reset, and forget the pad on the phone | Fixed by the reset; the exact cause is a best guess |
| Pad no longer appears in the phone's scan after forgetting it on the phone | The pad still holds its side of the pairing, most likely, so it waits for that phone instead of advertising to new devices. | Same reset | Fixed by the reset; the exact cause is a best guess |
| Keys do nothing on the phone while USB is plugged in | ZMK sends keys over USB whenever a cable is attached. | `CONFIG_ZMK_USB=n` keeps keys on Bluetooth. The USB log port stays enabled in debug builds. | Fixed by the change |
| `cp` reports an I/O error when flashing | The board reboots as the copy ends. The flash worked. | `just fw-flash` checks that the drive went away | Seen twice; the board came up on the new firmware both times |
| Pad stays on NO LINK after pairing | Correct. Pairing only sets up the keys; status starts once Open Camera connects, and that app has to have this pad selected as its remote device. | Select the pad in the Open Camera build's remote settings | Worked |
| NO LINK on a replacement board, even though HID keys work | Open Camera saves the remote's Bluetooth address, and a new board has a different one. | Select the new board in the remote settings | The code saves the address; selecting it again was part of the reset that fixed this |

## Still unexplained

- **"Needs a special app to pair":** the phone said this once, on the first attempt with
  the old settings. It did not come back after the reset.
- **Reading the USB log:** we never got a log line from a ZMK debug build. A raw read and a
  background `arduino-cli monitor` both captured nothing, which may only mean nothing was
  being logged. Try `arduino-cli monitor` in a real terminal first.
- **Volume keys on the replacement board:** they did nothing while `KP_PLUS` and `KP_MINUS`
  still worked, then worked after a reset and fresh pairing. We don't know whether the reset
  was the fix, because nothing narrowed it down first.
- **Slow serial port:** macOS took minutes to finish setting up the board on USB after one
  flash. An "allow accessory" prompt is a guess, not something we saw.

## Working method

- **A green build proves nothing about boot.** Only flashing does.
- **Test on real hardware before writing the explanation.** Bisect with several variants
  from one build (`just fw-build-debug <name> -DCONFIG_...`) instead of guessing one fix
  at a time, and keep unproven fixes off `main`.
- **Local Docker builds are cheap:** one image, one volume, and rebuilds in minutes.
- **Docker's disk can be the limit.** The full ZMK manifest is several GB of other chips'
  support, so `config/west.yml` allows only what a nice!nano needs. When a new feature
  needs another module, Kconfig warnings stop the build (that is how `mbedtls` got added).
- **Reused boards:** run `just fw-reset` first, every time.

## Check on any new board

1. Run `just fw-reset`; forget old entries on the phone.
2. The OLED shows NO LINK, and the phone sees `camkeypad` and pairs.
3. Select the pad in Open Camera: the pad shows IDLE.
4. Record from the phone: time counts on the pad.
5. Press the keys with USB plugged in: they still control the phone. Pausing needs Open
   Camera's volume keys option set to the pause action.
