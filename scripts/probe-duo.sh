#!/bin/sh
set -u

section() { printf '\n== %s ==\n' "$1"; }
run_if_present() {
    command -v "$1" >/dev/null 2>&1 && "$@" || printf '%s: unavailable\n' "$1"
}

section hostname; run_if_present hostname
section kernel; run_if_present uname -a
section memory; [ -r /proc/meminfo ] && sed -n '1,12p' /proc/meminfo || printf 'unavailable\n'
section mounts; run_if_present mount
section filesystems; run_if_present df -h
section network; run_if_present ip addr
section usb; run_if_present lsusb
section serial-devices; ls -l /dev/tty* 2>/dev/null || printf 'unavailable\n'
section modules; [ -r /proc/modules ] && cat /proc/modules || printf 'unavailable\n'
section dmesg-usb-uart
if command -v dmesg >/dev/null 2>&1; then
    dmesg 2>/dev/null | grep -Ei 'usb|dwc|gadget|midi|tty|uart|serial' || true
else
    printf 'dmesg: unavailable\n'
fi
