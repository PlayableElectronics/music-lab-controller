# Music Lab Controller

Headless-first control plane for a music and electronics lab, targeting the
original Milk-V Duo with CV1800B and 64 MB RAM. The Duo coordinates devices;
it is not the lab's main audio DSP or synthesis computer.

The initial target is a stock official Milk-V Duo Buildroot image, accessed
from macOS over the board's USB-C gadget/network connection. Later milestones
can add USB host/hub discovery, UART routing, network devices, MIDI/control
routing, presets, monitoring, and a terminal interface.

```text
Mac / PicoCalc / serial terminal
              |
          SSH / UART
              |
        Milk-V Duo / labd
       +------+------+------+
       |             |      |
   USB/CDC         UART   TCP/UDP
       |             |      |
 Daisy/RP2040   iCEBreaker  RPi
```

The long-term software boundary is a persistent `labd` daemon with a small
client API. `lab` and a future ANSI TUI are clients, not owners of device
state, so operation survives an SSH disconnect. `tuiui` is an option for a
later experiment, not an initial dependency.

## First milestone

1. Flash and boot the official 64 MB Buildroot image.
2. Confirm USB networking from macOS.
3. SSH to `root@192.168.42.1`.
4. Run `scripts/probe-duo.sh` on the board.
5. Inspect actual UART and USB-host support before customizing the image.

See [docs/bringup.md](docs/bringup.md), [docs/hardware.md](docs/hardware.md),
and [docs/architecture.md](docs/architecture.md).

## First CLI

Install the small BusyBox-compatible status command over SSH:

```sh
./scripts/install-lab.sh
```

Then run it remotely:

```sh
ssh root@192.168.42.1 lab status
```

Override the SSH target when needed:

```sh
LAB_HOST=root@192.168.42.1 ./scripts/install-lab.sh
```
