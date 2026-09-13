# USB testing

No downstream USB device has been tested yet. During initial bring-up the
stock Duo reported only:

```text
Bus 001 Device 001: ID 1d6b:0002
```

This is the root hub, not a musical device. After connecting the powered hub,
record the actual output of:

```sh
lsusb
dmesg | tail -100
find /sys/bus/usb/devices -maxdepth 2 -type f 2>/dev/null | head -100
lab devices
```

Then plug and unplug one real USB CDC, USB-serial, or USB-MIDI device and
repeat `lab devices` without restarting `labd`. Do not record a device name or
class here until it has been observed on this board.

The latest inspection of `daisies` also showed only the Pi's own root hub and
internal hub/Ethernet devices. It did not show the Duo as a USB device, so no
host-mode or downstream-device result is claimed yet.

## 2026-09-13 bridge observation

The Duo is now enumerated on `daisies` in its existing USB gadget mode:

```text
ID 3346:100c Cvitek NCM
usb0 192.168.42.180/24 on daisies
/dev/ttyACM0  usb-Cvitek_NCM_0123456789-if02
```

The composite gadget exposes CDC NCM networking and CDC ACM. This proves the
development USB bridge and transport drivers, but it is not USB host mode and
no downstream hub/device has been tested on the Duo. The remaining host-mode
test must wait for a proven independent TTL UART recovery console.
