java_package := "net.sourceforge.opencamera.remotecontrol"
zmk_image := "zmkfirmware/zmk-build-arm:stable"
zmk_volume := "camkeypad-zmk-west"

[private]
default:
    @just --list

# Regenerate the C header and Java class from protocol/protocol.toml
protocol-gen:
    python3 -I protocol/gen.py --in protocol/protocol.toml --out-dir protocol/generated --java-package {{java_package}}

# Fail if protocol/generated or the Arduino sketch's header is out of date with protocol/protocol.toml
protocol-check:
    just_scripts/protocol-check.sh {{java_package}}
    just_scripts/protocol-check-sketch.sh

# Copy the generated Java class into the Open Camera fork
protocol-sync-java opencamera_dir:
    just_scripts/protocol-sync-java.sh {{opencamera_dir}} {{java_package}}

# Copy the generated C header into an Arduino sketch folder
protocol-sync-c sketch_dir:
    just_scripts/protocol-sync-c.sh {{sketch_dir}} {{java_package}}

# Download the UF2s from the latest successful GitHub Actions build into build/firmware
fw-fetch:
    just_scripts/fw-fetch.sh

# Copy a UF2 onto a nice!nano in bootloader mode (double-tap reset first)
fw-flash uf2:
    just_scripts/fw-flash.sh {{uf2}}

# Erase the pad's pairings and saved settings, then flash a UF2 (default: latest CI firmware); start in bootloader mode
fw-reset *uf2:
    just_scripts/fw-reset.sh {{uf2}}

# Fetch ZMK and Zephyr into a Docker volume for local builds (first run downloads a few GB)
fw-setup:
    just_scripts/fw-setup.sh {{zmk_image}} {{zmk_volume}}

# Build a USB-logging firmware in Docker to build/local/<name>.uf2; extra args go to CMake
fw-build-debug name *cmake_args:
    just_scripts/fw-build-debug.sh {{zmk_image}} {{zmk_volume}} {{name}} {{cmake_args}}
