# Duo image

Target: original Milk-V Duo, CV1800B, 64 MB.

## Phase 0

Use a known-good official Milk-V Buildroot image unchanged. Verify:

- SD boot
- USB networking
- SSH from macOS
- memory available to Linux
- UART
- USB host mode/hub behavior

Record the exact upstream release and checksums before making a custom image.

## Phase 1

Pin the appropriate Milk-V Buildroot SDK revision and make the image reproducible from this repository using scripts/config overlays rather than vendoring the complete SDK.

Priorities for the custom image:

- SSH
- USB gadget networking for Mac administration
- USB host support needed by the lab
- UART tools
- minimal process/memory footprint
- reclaim multimedia/camera-reserved memory where safely possible
- install `labd` and `lab`

Do not add a graphical desktop. TUI experiments are optional packages layered above the basic controller.
