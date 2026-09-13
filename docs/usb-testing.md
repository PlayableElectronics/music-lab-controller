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

