#!/usr/bin/env bash
# Copy a UF2 onto a nice!nano in bootloader mode. The board reboots as the copy ends,
# which can make cp report an I/O error, so success means the drive has gone away.
set -uo pipefail
uf2=$1
drive=/Volumes/NICENANO

[ -d "$drive" ] || { echo "$drive not mounted: double-tap reset first" >&2; exit 1; }
cp "$uf2" "$drive/" 2> /dev/null
sleep 3
if [ -d "$drive" ]; then
    echo "$drive is still mounted: the flash did not take" >&2
    exit 1
fi
