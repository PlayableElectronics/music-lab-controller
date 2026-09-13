# UART console status

The UART recovery path is not yet proven. The current `daisies` inspection
found no `/dev/serial*`, `/dev/ttyAMA*`, `/dev/ttyUSB*`, or `/dev/ttyACM*`
device, so no USB-TTL adapter or direct Duo console was detected.

Do not change the Duo USB role until this path is available and tested.

## Required wiring

Use a verified 3.3 V TTL adapter and the pin mapping for the exact original
Milk-V Duo board revision. Connect common ground, Duo TX to adapter RX, and
Duo RX to adapter TX. Do not connect 5 V or guess GPIO pins. The adapter's
serial device must be identified from `daisies` after it is connected.

The intended terminal path is:

```text
Mac -> SSH -> daisies -> serial terminal -> Duo
```

Once a device appears, inspect its permissions and use an installed serial
terminal at 115200 8N1, for example:

```sh
ssh daisies
picocom --baud 115200 /dev/ttyUSB0
```

Replace `/dev/ttyUSB0` only with the actual verified device. Confirm the Duo
console with `cat /proc/cmdline`, `dmesg | grep -i tty`, and
`cat /etc/inittab` before relying on it for recovery.

## Safety gate

Before any USB role experiment, verify over this console that the Duo can run
`lab status`, `lab devices`, and `reboot`, with boot output and login visible
again after reboot.
