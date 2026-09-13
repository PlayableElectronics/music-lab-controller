# Architecture

## Purpose

The Milk-V Duo is the control plane for the music lab. Audio engines remain independent and can continue operating if the controller UI disconnects or restarts.

## Layers

```text
TUI / CLI / automation
        |
       labd
        |
 transport adapters
  /     |      |    \
USB   UART   MIDI   network
 |      |      |       |
MCUs   FPGA controllers other Linux nodes
```

## `labd`

`labd` will eventually own:

- device discovery and stable logical device names
- transport connections
- current device state
- command dispatch
- reconnect handling
- events for clients

The first implementation should stay deliberately small. A Unix-domain socket and a line-oriented protocol are enough for bring-up.

## Frontends

`lab` is the first frontend and must work in a plain terminal. `lab-tui` comes later and talks to the same daemon API. tuiui is an optional desktop/shell experiment, not part of the daemon architecture.

## Transport principle

Device identity must not depend on enumeration order such as `/dev/ttyACM0`. USB devices should eventually be mapped from VID/PID/serial/path information to stable logical names.

## First end-to-end path

```text
Mac
 |
USB-C gadget networking
 |
SSH
 |
Milk-V Duo
 |
lab CLI
 |
labd
 |
UART or USB CDC
 |
one musical device
```

Prove this path before adding routing, MIDI, OSC, presets or elaborate UI work.
