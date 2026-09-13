# Initial bring-up

This checklist intentionally starts with the official Milk-V Duo 64 MB image.
Do not replace U-Boot, OpenSBI, the kernel, or Buildroot until a stock system
is known to boot.

1. Obtain the official image for the original CV1800B Duo.
2. Insert a suitable SD card and identify it explicitly with `diskutil list`.
3. Flash it using the guarded [scripts/flash.sh](../scripts/flash.sh).
4. Boot the Duo and connect its USB-C data port to the Mac.
5. Confirm the USB network interface and ping `192.168.42.1`.
6. Run `scripts/connect.sh 192.168.42.1`.
7. On the Duo, run `scripts/probe-duo.sh`.
8. From the Mac, install the controller with `./scripts/install.sh` once a
   RISC-V Linux compiler is available.
9. Confirm `ssh root@192.168.42.1 lab ping` and `lab devices`.

Useful inspection commands after SSH:

```sh
uname -a
cat /proc/cpuinfo
cat /proc/meminfo
df -h
mount
ip addr
lsusb
dmesg
ls -l /dev/tty*
```

Record actual outputs before changing configuration. In particular, verify
which USB controller mode, class drivers, UARTs, and modules the stock image
provides. Do not assume USB host or USB MIDI support.

For the current stock image, the SSH server does not include
`/usr/libexec/sftp-server`; installers must use legacy SCP (`scp -O`). The
project install script handles this compatibility detail.
