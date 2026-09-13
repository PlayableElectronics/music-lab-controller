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

### Verified UART recovery console

- `daisies` is a Raspberry Pi 3 Model B+ using `/dev/serial0` ->
  `/dev/ttyAMA0`; GPIO14/TXD0 (physical pin 8) connects to Duo UART0 RX
  (original Duo physical pin 10), and GPIO15/RXD0 (physical pin 10) connects
  to Duo UART0 TX (original Duo physical pin 8). Ground is Pi physical pin 6
  to Duo physical pin 14/20. The connection is 3.3 V TTL, crossed TX/RX,
  115200 8N1.
- `daisies` required `enable_uart=1`, `dtoverlay=disable-bt`, and stopping
  `serial-getty@ttyAMA0` so the serial terminal could own the port.
- The Duo boot console is `ttyS0` (`console=ttyS0,115200 earlycon=sbi`). An
  interactive login was verified over the direct UART. Over that console,
  `lab ping`, `lab status`, and `lab devices` all succeeded.
- The Duo was rebooted while the UART reader remained open. Boot output and
  login returned; `lab ping` returned `OK PONG`, `pidof labd` returned PID 163,
  and the status/device commands succeeded. This satisfies the independent
  recovery safety gate.
- The live Duo USB role was not changed. A read-only inspection found the
  original-image helper `/mnt/system/usb-host.sh`, which writes `host` or
  `device` to `/proc/cviusb/otg_role`; see `docs/usb-role.md`. No role command
  was executed.

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

## 2026-09-13 — Duo bridge and first labd deployment

- The Duo is now visible through `daisies` as USB `3346:100c Cvitek NCM`.
- `daisies` receives `usb0=192.168.42.180/24`; the Duo remains reachable at
  `192.168.42.1` through that bridge.
- The composite gadget also creates `/dev/ttyACM0` on `daisies`, but this is
  the Duo USB CDC ACM function, not an independent 3.3 V TTL UART recovery
  console.
- The RISC-V binaries were built on `daisies`, transferred through the bridge,
  and executed on the real Duo. `lab ping`, `lab status`, and `lab devices`
  succeeded.
- `labd` is started by `/etc/init.d/S70labd` and has PID 1170 during the
  measurement. The measured `/proc/1170/status` values were `VmSize=1132 kB`
  and `VmRSS=640 kB`; `/usr/local/lib/music-lab/labd` is 545 KiB.
- `lab devices` currently discovers the five onboard UART nodes: `ttyS0`
  through `ttyS4`. No downstream USB musical device is attached.
- A separate SSH connection confirmed the daemon remains running after the
  installing SSH session ended. Reboot persistence and UART-session
  persistence remain untested because the independent TTL console safety gate
  is not yet satisfied.
- No USB gadget-to-host role command has been identified or executed.
