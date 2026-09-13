# Hardware

## Controller

Initial target:

- Milk-V Duo, original 64 MB version
- CV1800B SoC
- microSD boot
- macOS development host
- USB-C networking for administration

## Planned links

### Mac <-> Duo

First use the firmware's USB networking and SSH path. This is the development/admin connection.

### Duo <-> USB hub

USB host mode is planned so a powered hub can attach USB CDC, USB MIDI and other supported control devices. Verify kernel/firmware support on the exact image before depending on a USB class.

### Duo <-> FPGA/MCU

UART is the simplest initial hardware control link. During bring-up prefer a human-readable protocol, for example:

```text
@FPGA GET STATUS
@FPGA SET FREQ 110.0
@FPGA STRIKE
```

Binary framing can be introduced later if measurements show it is necessary.

## Candidate controlled devices

- iCEBreaker FPGA
- Daisy
- RP2040
- ESP32
- Raspberry Pi/Patchbox nodes

The Duo should not assume these devices share an audio clock. It controls and routes metadata/commands; audio transport and synchronization are separate concerns.
