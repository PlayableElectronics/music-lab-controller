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

## Persistent controller milestone

The current control-plane implementation adds a small POSIX `labd` daemon,
an ANSI terminal monitor, and a client-backed `lab` command. After building
with a RISC-V Linux compiler and installing with `scripts/install.sh`:

```sh
ssh root@192.168.42.1 lab ping
ssh root@192.168.42.1 lab devices
ssh -t root@192.168.42.1 lab-tui
```

`labd` listens on `/tmp/labd.sock`, refreshes serial devices periodically,
and is started by the BusyBox init script installed as `S70labd`. The daemon
does not depend on an SSH session.

`scripts/install.sh` expects a runnable `riscv64` Linux C compiler. The
official Milk-V host-tools compiler is a Linux x86_64 executable; it cannot
run natively on an Apple Silicon macOS host. Set `LAB_CC` to a compiler from
an available Linux x86_64 build host or VM, for example:

```sh
LAB_CC=/path/to/riscv64-unknown-linux-musl-gcc ./scripts/install.sh
```

The installer uses legacy SCP (`scp -O`) because the stock Dropbear image has
no SFTP server. It changes only the project files under `/usr/local` and the
project init/config paths under `/etc`; it does not alter general OS
configuration.
