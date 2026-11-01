java_package := "net.sourceforge.opencamera.remotecontrol"

[private]
default:
    @just --list

# Regenerate the C header and Java class from protocol/protocol.toml
protocol-gen:
    python3 -I protocol/gen.py --in protocol/protocol.toml --out-dir protocol/generated --java-package {{java_package}}

# Fail if protocol/generated is out of date with protocol/protocol.toml
protocol-check:
    just_scripts/protocol-check.sh {{java_package}}

# Copy the generated protocol code into the Open Camera fork and the Arduino sketch folder
protocol-sync opencamera_dir arduino_sketch_dir:
    just_scripts/protocol-sync.sh {{opencamera_dir}} {{arduino_sketch_dir}} {{java_package}}
