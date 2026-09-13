# UART console status

The independent UART recovery path is verified on the original Duo and has
passed the reboot safety test. The Duo boot console and login are available
through UART0 while its USB-C gadget/network connection remains unchanged.

## Required wiring

The tested direct connection is between the Pi `daisies` and the original
26-pin Duo header:

| Signal | `daisies` | Duo |
|---|---|---|
| Pi TXD0 | GPIO14, physical pin 8 | Duo physical pin 10, A17 / UART0_RX |
| Pi RXD0 | GPIO15, physical pin 10 | Duo physical pin 8, A16 / UART0_TX |
| Ground | physical pin 6 | Duo physical pin 14 or 20 |

This is a 3.3 V TTL connection: TX and RX are crossed and ground is shared.
No 5 V connection is used. The original Duo pin names and UART0 header
mapping are documented by [Milk-V](https://milkv.io/docs/duo/getting-started/arduino).

The intended terminal path is:

```text
Mac -> SSH -> daisies -> serial terminal -> Duo
```

On `daisies`, the tested serial device is `/dev/serial0`, resolving to
`/dev/ttyAMA0`. The Pi was configured with `enable_uart=1` and
`dtoverlay=disable-bt`; the serial getty was stopped so the terminal could
own the port. The verified serial format is 115200 baud, 8 data bits, no
parity, 1 stop bit (8N1), raw mode. `picocom`, `minicom`, and `screen` are not
installed on the current Pi, but BusyBox provides the lightweight interactive
terminal `microcom`. The reproducible command is:

```sh
ssh pi@192.168.1.50
sudo systemctl stop serial-getty@ttyAMA0.service
sudo busybox microcom -s 115200 /dev/serial0
```

The `serial-getty` stop is needed only when that service is active and owns
the port. Exit `microcom` with its BusyBox escape sequence (normally
`Ctrl-X`, depending on the installed BusyBox build).

The Duo reports `console=ttyS0,115200 earlycon=sbi`; its BusyBox inittab runs
the console getty at 115200. Interactive login was observed, followed by
successful execution of `uname -a`, `lab ping`, `lab status`, and
`lab devices`.

## Safety gate

Before any USB role experiment, verify over this console that the Duo can run
`lab status`, `lab devices`, and `reboot`, with boot output and login visible
again after reboot. This was completed on 2026-09-13: after reboot, `lab ping`
returned `OK PONG`, `pidof labd` showed the automatically started daemon, and
the status/device commands succeeded.
