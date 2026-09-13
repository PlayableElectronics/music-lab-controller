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
