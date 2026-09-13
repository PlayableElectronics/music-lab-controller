# Hardware

## Target

- Milk-V Duo, original board
- CV1800B SoC
- 64 MB RAM
- official Milk-V Duo Buildroot environment

Do not use the Duo 256M or Duo S assumptions for this project. RAM is a
first-order design constraint.

## Expected interfaces

- USB-C gadget/network interface for development-host SSH
- USB host and a powered hub, to be verified on the stock image
- UART and GPIO
- Ethernet/network when available
- USB MIDI if the kernel and class support are present

Possible controlled devices include Daisy, iCEBreaker FPGA, RP2040, ESP32,
Raspberry Pi systems, USB MIDI devices, and plain serial peripherals.

The Duo is a control-plane computer. Audio DSP, synthesis, and FPGA workloads
remain on dedicated hardware.

## Stock image observed during bring-up

The running board reports Linux 5.10.4 on `riscv64`, approximately 56,348 kB
visible memory, and a USB-C gadget interface at `192.168.42.1`. It exposes
`/dev/ttyGS0` and `/dev/ttyS0` through `/dev/ttyS4`. The stock image has
BusyBox init, Dropbear, `/usr/bin/nc`, and `/sbin/mdev`; `gcc`, `cc`, `socat`,
and `inotifyd` are absent. With no powered hub or downstream device attached,
`lsusb` showed only the USB root hub. USB-host/class support must therefore be
verified with the powered hub and a real device rather than assumed.

The stock image uses `/etc/inittab` to run `/etc/init.d/rcS`, which starts
numbered scripts such as `S99user`. The project startup script is named
`S70labd` and is installed only by the explicit project installer; no stock
boot files are changed by the image scaffolding.

## Daisies bridge observation

`daisies` is a Raspberry Pi running Debian 13 on `aarch64` at the currently
known address `192.168.1.50`. It has native `/usr/bin/gcc` and can host the
Debian `riscv64-linux-gnu` cross compiler. A non-root extracted toolchain was
used successfully for a strict static build; see `docs/notes.md`.

The Pi's usable UART is `/dev/serial0` (`/dev/ttyAMA0`) after enabling the
UART and disabling the Bluetooth overlay. The verified direct wiring uses Pi
GPIO14/TXD0 and GPIO15/RXD0 to the original Duo header's UART0 pins (Duo
physical pins 10/RX and 8/TX respectively), plus common ground. This UART
console has been used to log in, run the lab commands, and observe a complete
Duo reboot. It is now the recovery path for any future USB-role experiment.

The Duo is also visible to `daisies` in its current gadget mode as USB
`3346:100c` with `usb0=192.168.42.180/24` on the Pi. Downstream host/hub
capability has not yet been tested.
