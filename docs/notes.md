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

## 2026-09-13 — daisies bridge attempt

- `daisies` is a Raspberry Pi arm64 running Debian 13, reachable at
  `pi@192.168.1.50` (`daisies.local` did not resolve from the Mac).
- Native GCC is present. Debian's `gcc-riscv64-linux-gnu` package is available
  but installing it requires the Pi user's sudo password.
- As a non-root workaround, the compiler and development packages were
  downloaded/extracted under `/tmp/riscv-root` on `daisies`.
- Exact compiler used:
  `/tmp/riscv-root/usr/bin/riscv64-linux-gnu-gcc-14`.
- Successful command used `-std=c99 -Os -Wall -Wextra -Werror -static` with
  the extracted sysroot; both binaries are RISC-V static ELF executables.
- Sizes on `daisies`: `labd` 545 KiB and `lab-client` 477 KiB after stripping.
- The latest USB inspection found only the Pi's own hub chain and no Duo;
  there were no serial adapter nodes. The Duo's USB-C SSH path also timed
  out, so no binary was deployed or executed on the Duo.
- No UART console or USB role switch was attempted. The safety gate remains
  active.
