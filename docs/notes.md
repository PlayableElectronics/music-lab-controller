# Development notes

## 2026-09-13 — control-plane milestone

- Target: original Milk-V Duo, CV1800B, 64 MB, stock Buildroot Linux.
- Live board: Linux 5.10.4, `riscv64`, USB-C network at `192.168.42.1`.
- Observed serial nodes: `/dev/ttyS0` through `/dev/ttyS4`.
- Observed USB: root hub only; a powered hub and downstream musical device
  still need to be tested.
- Available target tools include BusyBox `nc` and `mdev`; no target compiler,
  `socat`, or `inotifyd` was present.
- Added fixed-size POSIX `labd`, Unix-socket `lab` client, serial transport,
  periodic `/sys` discovery, BusyBox startup script, and ANSI `lab-tui`.
- Local integration and shell syntax tests pass.
- The official Milk-V cross-compiler downloaded for investigation is a Linux
  x86_64 executable and cannot run directly on this Apple Silicon Mac.

### Next bring-up

Use a Linux x86_64 build host or VM with the RISC-V musl compiler, then run:

```sh
LAB_CC=/path/to/riscv64-unknown-linux-musl-gcc ./scripts/install.sh
ssh root@192.168.42.1 lab ping
ssh root@192.168.42.1 lab devices
ssh -t root@192.168.42.1 lab-tui
```

After installation, connect the powered USB hub and record actual `lsusb`,
`dmesg`, sysfs, and `lab devices` output in `docs/usb-testing.md`.
