# Music Lab Controller

Headless-first music-lab control plane for the original **Milk-V Duo (CV1800B, 64 MB RAM)**.

The Duo is an always-on controller/router rather than the main DSP machine. The frontend should be reachable over SSH, local console, or eventually UART, while `labd` keeps device state alive independently of the terminal session.

## Milestone 1

1. Boot minimal Buildroot Linux on Milk-V Duo 64 MB.
2. Connect Mac -> USB-C networking -> SSH -> Duo.
3. Run a minimal `lab` CLI.
4. Talk to one external device over UART or USB CDC.
5. Keep the interface terminal-friendly from the beginning.

## Planned architecture

```text
Mac / PicoCalc / serial terminal
            |
        SSH / UART
            |
       Milk-V Duo
            |
      +-----+-----+
      |           |
   lab-tui      lab CLI
      |           |
      +-----+-----+
            |
           labd
      +-----+------+-------+
      |            |       |
    USB CDC       UART   TCP/UDP
      |            |       |
 Daisy/RP2040  iCEBreaker  RPi/etc
```

## Layout

- `docs/` - architecture and hardware notes
- `image/` - reproducible Duo image work
- `scripts/` - host-side flash/connect helpers
- `labd/` - controller daemon
- `lab/` - CLI
- `tui/` - terminal frontend experiments
- `devices/` - device adapters/protocol notes

## Design rules

- Keep Linux small and predictable.
- Keep hard realtime audio/DSP on the appropriate Daisy/FPGA/MCU devices initially.
- Prefer debuggable plain-text control protocols during bring-up.
- Do not make tuiui or any other TUI framework a dependency of `labd`.
- Disconnecting SSH must never stop the music system.
