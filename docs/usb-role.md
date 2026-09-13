# Duo USB role investigation

No role switch has been performed. The Duo is currently reachable through
its USB-C gadget mode via `daisies` (`usb0` on the Pi, `192.168.42.1` on the
Duo). The independent TTL UART recovery path is now proven, including a Duo
reboot with boot output, login, and automatic `labd` restart visible on the
serial console.

When the Duo is reachable, record the actual output of:

```sh
cat /proc/cmdline
dmesg | grep -i tty
cat /etc/inittab
find / -iname '*usb*host*' 2>/dev/null
find / -iname '*usb*device*' 2>/dev/null
grep -R 'dwc\|otg\|gadget\|host' /etc /root /mnt /usr 2>/dev/null | head -200
ls /sys/class/udc
find /sys -path '*usb_role*' -o -path '*role*' 2>/dev/null
```

The original image contains the following board-specific helper:

```sh
/mnt/system/usb-host.sh host
/mnt/system/usb-host.sh device
```

Its implementation was read on the live board. `host` loads
`/mnt/system/ko/dwc2.ko` and writes `host` to `/proc/cviusb/otg_role`;
`device` writes `device` to the same node. The current live value is
`device`, and `/sys/class/udc/4340000.usb` is present. The helper is specific
to this stock image and is not being treated as guidance for Duo S or Duo
256M. Neither command has been executed yet.

The exact observed restore command, if a host-mode test is later performed,
is `/mnt/system/usb-host.sh device`. The host transition must be performed
only from the proven UART console, with the serial session kept open.

The current verified USB observation is the Duo composite gadget (`3346:100c`)
with CDC NCM and CDC ACM interfaces. This is not evidence of downstream USB
host enumeration. The UART safety gate is now satisfied, so the project is
safe to attempt a controlled powered-hub host test. A first controlled attempt
was made from UART on 2026-09-13 using `/etc/uhubon.sh host`; it disconnected
the gadget but failed with:

```text
insmod: can't insert '/mnt/system/ko/dwc2.ko': No such file or directory
```

The role was restored from UART with `/etc/uhubon.sh device`. The gadget
returned, `ROLE=device`, `lab ping` returned `OK PONG`, `labd` restarted/was
running, and the five onboard UART nodes were still discovered. No downstream
hub or USB host enumeration was observed. Do not retry host mode until the
missing module/stock-image host support is resolved.
