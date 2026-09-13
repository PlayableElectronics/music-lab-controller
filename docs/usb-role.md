# Duo USB role investigation

No role switch has been performed. The USB-C gadget SSH path was not
available during the latest check, and the Duo was not enumerated on
`daisies`. The safety requirement is therefore still active: establish and
verify UART recovery before changing the USB role.

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

The current verified USB observation on `daisies` is only its own root hub
and internal hub/Ethernet devices; it is not evidence of Duo host support.
