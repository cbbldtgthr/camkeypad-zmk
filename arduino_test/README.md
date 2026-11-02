# camkeypad arduino_test

Bench sketch that answers one question without any ZMK firmware: does the phone let an
app talk GATT to the pad while Android's HID host is using it as a keyboard?

Runs on a bare nice!nano v2 over USB, no other hardware. `camkeypad_protocol.h` is
generated; run `just protocol-sync-c arduino_test` from the repo root, never edit it here.

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

[nRF Connect for Mobile](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp)
(Nordic Semiconductor, free) is a generic BLE client: it can connect to the pad, browse
its GATT services and write bytes by hand, standing in for Open Camera.

1. **Pair.** Forget any earlier pairing of this board on the phone (e.g. a ZMK keyboard;
   the bond keys will clash). Pair `camkeypad-test` from Android Bluetooth settings. The
   monitor prints `connected …`.
2. **Keys.** Open Open Camera in video mode and type `u` in the monitor (no Enter
   needed). Recording starts; `u` again stops it.
3. **GATT alongside HID.** In nRF Connect open the **Bonded** tab (a pad connected as a
   keyboard usually stops advertising, so it may not appear under Scanner), tap
   **Connect** on `camkeypad-test`, expand the service `1d0d0dc8-…`, tap the write arrow
   on characteristic `18ec2b07-…` and send `01 01 10 27 00 00`. The monitor shows the
   bytes and `RECORDING 00:10`. With nRF Connect still connected, `u` must still drive
   Open Camera.
4. **After disconnecting.** Disconnect in nRF Connect; `u` must still drive Open Camera.

All four pass: the design holds on this phone. After re-flashing between this sketch and
ZMK, forget the device on the phone and pair again.

### If it fails

- **Custom service missing:** the phone's GATT cache is stale. In nRF Connect use
  *Refresh services* from the menu, or toggle Bluetooth.
- **Write rejected:** check the characteristic's write permission in the sketch.
- **Keys stop when nRF Connect connects:** a vendor BLE stack quirk. The fallback is
  sending keys as GATT notifications too, as Open Camera's Kraken remote support does.

### Results

| Phone | Android | Result |
|---|---|---|
| Samsung Galaxy A71 | 13 | All four pass |
