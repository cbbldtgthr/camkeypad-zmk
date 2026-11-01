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

# Copy the generated Java class into the Open Camera fork
protocol-sync-java opencamera_dir:
    just_scripts/protocol-sync-java.sh {{opencamera_dir}} {{java_package}}

# Copy the generated C header into an Arduino sketch folder
protocol-sync-c sketch_dir:
    just_scripts/protocol-sync-c.sh {{sketch_dir}} {{java_package}}
