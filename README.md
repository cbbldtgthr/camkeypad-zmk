# camkeypad

A Bluetooth keypad for filming with Open Camera on Android, which shows the phone's
**real** recording state on the pad itself.

## Why

The keys alone are not enough. A pad that sends "volume up = start/stop" and
"volume down = pause/resume" cannot know what the phone actually did with them, so it
is easy to lose track and end up not recording. The pad needs to hear the truth from
the phone.

## Principles

- **The phone is the source of truth.** Open Camera pushes its recording state to the
  pad; the pad never infers state from its own key presses.
- **A stale display must not lie.** If the pad stops hearing from the phone, it shows
  that the link is lost, not the last known state.
- **Keys keep working without the status link.** Keys are plain HID; status runs on a
  separate GATT service, so losing one does not take down the other.
- **One definition of the protocol.** The wire protocol is defined once, as data, in
  `protocol/protocol.toml`. Code for every consumer is generated from it; nobody types
  protocol constants by hand.

## The parts

| Part | Role |
|---|---|
| This repo | ZMK firmware for the pad (nice!nano v2), and the protocol definition |
| [`opencamera-camkeypad`](https://github.com/cbbldtgthr/opencamera-camkeypad) | Open Camera fork that pushes recording state to the pad |
| [`arduino_test/`](arduino_test/README.md) | Bench sketch that checks a phone lets HID and a custom GATT service coexist, on a bare nice!nano |

Run `just` to list the available commands. Before debugging the pad, read
[`docs/bring-up-learnings.md`](docs/bring-up-learnings.md).
