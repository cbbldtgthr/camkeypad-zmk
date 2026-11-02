#!/usr/bin/env bash
# Erase the pad's saved settings (pairings, Bluetooth name), then flash firmware back on.
# Takes two bootloader entries: settings_reset does its work and leaves the board idle.
set -euo pipefail
uf2=${1:-build/firmware/firmware/camkeypad-nice_nano__zmk-zmk.uf2}
reset_uf2=build/firmware/firmware/settings_reset-nice_nano__zmk-zmk.uf2
drive=/Volumes/NICENANO
wait_seconds=300

[ -d "$drive" ] || { echo "$drive not mounted: double-tap reset first" >&2; exit 1; }
[ -f "$reset_uf2" ] || just_scripts/fw-fetch.sh
[ -f "$uf2" ] || { echo "not found: $uf2" >&2; exit 1; }

just_scripts/fw-flash.sh "$reset_uf2"
echo "Settings erased. Wait 5 seconds, then double-tap reset to flash $uf2"

waited=0
until [ -d "$drive" ]; do
    if [ "$waited" -ge "$wait_seconds" ]; then
        echo "$drive did not come back: double-tap reset, then run: just fw-flash $uf2" >&2
        exit 1
    fi
    sleep 1
    waited=$((waited + 1))
done
# Give macOS a moment to finish mounting before copying.
sleep 2
just_scripts/fw-flash.sh "$uf2"
echo "Done. Forget the pad on the phone if it is listed, pair it, then select it in Open Camera."
