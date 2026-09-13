# Duo power topology

Power wiring is deliberately unresolved until the original Duo schematic is
reviewed for the exact board revision. Do not connect Raspberry Pi 5 V to the
Duo’s `VBUS` or `VSYS` based on pin names alone.

The official original-Duo pinout identifies header pin 40 as `VBUS` and pin
39 as `VSYS`; the [official schematic](https://github.com/milkv-duo/duo-files/blob/main/duo/hardware/duo-schematic-v1.2.pdf)
shows the USB-C VBUS path and a diode connection into the system rail. That
establishes that these nets are not interchangeable signal pins, but it is
not by itself permission to tie two powered boards together.

Until the schematic-backed current/voltage and backfeed behavior is verified:

- keep the Duo powered by its normal USB-C supply;
- keep UART connected only at 3.3 V logic and common ground;
- do not feed Pi 5 V into Duo VBUS or VSYS;
- do not use a powered hub to backfeed the Duo;
- use the hub’s own supply only after the custom host-mode power path is
  validated.

The temporary safe plan is therefore independent Duo power plus the tested
UART recovery connection. A final host/hub wiring recommendation will be
added only after direct schematic inspection confirms the permitted VSYS
input range, VBUS direction, diode/isolation path, and hub VBUS behavior.
