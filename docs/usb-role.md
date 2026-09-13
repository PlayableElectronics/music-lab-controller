# Duo USB role investigation

No role switch has been performed. The Duo is currently reachable through
its USB-C gadget mode via `daisies` (`usb0` on the Pi, `192.168.42.1` on the
Duo). The safety requirement is still active: establish and verify an
independent TTL UART recovery path before changing the USB role.

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

Do not apply a host-mode command from another Duo variant. Document the
exact reversible command found on this original CV1800B image, and its exact
restore-to-gadget command, before testing either one.

The current verified USB observation is the Duo composite gadget (`3346:100c`)
with CDC NCM and CDC ACM interfaces. This is not evidence of Duo host support.
