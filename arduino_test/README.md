# camkeypad arduino_test

Throwaway sketch that answers one question before any ZMK work: does the phone let an
app talk GATT to the pad while Android's HID host is using it as a keyboard?

Runs on a bare nice!nano v2 over USB, no other hardware. `camkeypad_protocol.h` is
generated in `camkeypad-zmk`; refresh it from there, never edit it here.

## Run it

```bash
brew install arduino-cli   # the only global tool; the core lives in .arduino/
just setup
just compile
just ports                 # find the nice!nano's port; double-tap reset if it isn't listed
just upload /dev/cu.usbmodemXXXX
just monitor /dev/cu.usbmodemXXXX
```

## The test

1. Pair `camkeypad-test` in Android Bluetooth settings. Open Open Camera in video mode.
   Type `u` + Enter in the monitor; recording should start. `u` again stops it.
2. In nRF Connect, open the **Bonded** tab, connect to `camkeypad-test`, find the
   service `1d0d0dc8-…`, and write to characteristic `18ec2b07-…`:
   `01 01 10 27 00 00` should log `RECORDING 00:10`.
3. With nRF Connect still connected, `u` must still drive Open Camera.
4. Disconnect nRF Connect; `u` must still drive Open Camera.

All four pass: the design holds on this phone. After re-flashing between this sketch and
ZMK, forget the device on the phone and pair again.
