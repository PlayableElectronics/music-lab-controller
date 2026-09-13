# Architecture

```text
hardware transports (USB, UART, network)
                 |
                labd
                 |
             client API
              /       \
          lab CLI     ANSI TUI
```

`labd` will own discovery, logical device identity, routing, monitoring, and
persistent state. Clients request actions and subscribe to state; they do not
open devices directly or keep the authoritative state in an SSH session.

The first implementation should remain small: a plain text or line-oriented
Unix-socket API, fixed-size state, and BusyBox-compatible helpers. The daemon
must eventually continue controlling devices after a client disconnects.

Device names should be logical and stable, based on VID/PID, serial number,
topology, or an explicit handshake rather than `/dev/ttyACM0` alone.

The initial terminal UI must work with an ANSI/VT100 terminal. `tuiui` may be
evaluated later, but is not required for the first milestone.

## Development and deployment topology

During bring-up, `daisies` is a Raspberry Pi development/build and recovery
bridge:

```text
Mac --SSH--> daisies --UART--> Duo
                    |
                  build
```

The intended performance topology is separate from that bridge:

```text
Mac --Ethernet/Wi-Fi--> USB network adapter --> powered hub --> Duo
PicoCalc ----------------UART-------------------------------> Duo
                                                    |
                                             USB lab devices
```

USB gadget mode is temporary development access. USB host mode is reserved
for lab peripherals and network adapters. UART is the safe recovery path and
must be proven before changing USB roles.

## Current runtime boundary

The first daemon listens on `/tmp/labd.sock` and uses a small line protocol:
`PING`, `STATUS`, `DEVICES`, `DEVICE NAME`, and `REFRESH`. The client keeps no
device state of its own. `labd` refreshes `/sys/class/tty` periodically, so a
client reconnect sees current serial devices even after an SSH session ends.
