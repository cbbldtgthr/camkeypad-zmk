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

### Without the reset pin

Hold both zoom buttons for the service layer, then:

- **Start/stop** enters the bootloader, in place of the double-tap. The `NICENANO` drive only
  appears with USB plugged in; tap reset to get back out.
- **Pause, held for a second,** clears the pad's pairing, in place of steps 1 to 3 above when
  pairing is the only problem. Saved settings such as the Bluetooth name stay. Forget the pad
  on the phone and pair again; Open Camera reconnects by itself, because the board's address
  doesn't change.

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
| Wide 28pt letters (`O`, `V`) draw as dotted blocks while narrower ones are fine | LVGL allocates a buffer per glyph from its pool, and at 4096 bytes the largest ones most likely failed. | `CONFIG_LV_Z_MEM_POOL_SIZE=8192` | Going to 8192 fixed it. The failed allocation is inferred from the code and glyph sizes. |
| OLED stays dark though its VCC reads 3.3V; the USB log shows `ssd1306: Failed to initialize device!` at boot | SDA shorted to SCL. Two screens failed the same way. | Clear the short, then reset the pad, because the driver only tries at boot | Fixed by clearing the short |

## Still unexplained

- **"Needs a special app to pair":** the phone said this once, on the first attempt with
  the old settings. It did not come back after the reset.
- **Volume keys from the pad:** `C_VOL_UP` and `C_VOL_DN` came and went on the same board and
  firmware, while keyboard keys kept working. When they failed, the USB log showed them being
  sent with no errors, but the phone showed no volume bar. A reset and fresh pairing fixed it
  once, then didn't; unplugging the OLED didn't either. The phone's own volume buttons kept
  working. The pad now sends `F7` and `F8` instead, so this cause is still unknown.
- **What a clean USB log proves:** ZMK logs each key before sending it and drops keys without
  a word when there is no connection. To see that the app is connected, look for
  `write_status` lines every couple of seconds.
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
- **Read the USB log first.** Debug builds log to `/dev/cu.usbmodem*`, and reading the port
  from a script works. The first read after boot also returns buffered boot lines, where
  driver errors show up.

## Check on any new board

1. Run `just fw-reset`; forget old entries on the phone.
2. The OLED shows NO LINK, and the phone sees `camkeypad` and pairs.
3. Select the pad in Open Camera: the pad shows PHOTO or VIDEO, and the phone's battery.
4. Record from the phone: time counts on the pad.
5. Press the keys with USB plugged in: they still control the phone. Start/stop and pause
   need the Open Camera fork, which handles `F7` and `F8`.
