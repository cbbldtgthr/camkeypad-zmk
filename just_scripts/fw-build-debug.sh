#!/usr/bin/env bash
# Build a USB-logging firmware in Docker and copy it to build/local/<name>.uf2.
# Sources stay in the Docker volume; only the repo and the UF2 cross to the Mac.
set -euo pipefail
image=$1
volume=$2
name=$3
shift 3

mkdir -p build/local
docker run --rm --volume "$volume:/workspace" --volume "$PWD:/repo" \
    --workdir /workspace "$image" bash -euo pipefail -c '
        name=$1
        shift
        # Registers Zephyr with CMake in the home directory, which dies with the container.
        west zephyr-export > /dev/null
        west build --pristine always --source-dir zmk/app --build-dir "build/$name" \
            --board nice_nano//zmk --snippet zmk-usb-logging -- \
            -DSHIELD=camkeypad -DZMK_CONFIG=/repo/config -DZMK_EXTRA_MODULES=/repo "$@"
        cp "build/$name/zephyr/zmk.uf2" "/repo/build/local/$name.uf2"
    ' bash "$name" "$@"
echo "build/local/$name.uf2"
