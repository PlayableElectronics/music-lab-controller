#!/bin/sh
set -eu

usage() { printf 'Usage: %s IMAGE /dev/diskN\n' "$0" >&2; exit 2; }
[ "$#" -eq 2 ] || usage
IMAGE=$1
DISK=$2

case "$DISK" in
    /dev/disk[1-9]|/dev/disk[1-9][0-9]) ;;
    *) printf 'Refusing disk path: %s\nUse an explicit /dev/diskN (never disk0).\n' "$DISK" >&2; exit 1 ;;
esac
[ -f "$IMAGE" ] || { printf 'Image not found: %s\n' "$IMAGE" >&2; exit 1; }
command -v diskutil >/dev/null 2>&1 || { printf 'diskutil is required on macOS\n' >&2; exit 1; }
command -v dd >/dev/null 2>&1 || { printf 'dd is required\n' >&2; exit 1; }

printf '\n!!! DESTRUCTIVE SD CARD WRITE !!!\n'
printf 'Image: %s\nTarget: %s\n\n' "$IMAGE" "$DISK"
DISK_INFO=$(diskutil info "$DISK")
printf '%s\n' "$DISK_INFO"
case "$DISK_INFO" in
    *"Internal:                 Yes"*|*"Device Location:          Internal"*)
        printf 'Refusing an internal disk. Choose the explicitly identified SD card.\n' >&2
        exit 1
        ;;
esac
printf '\nType exactly: FLASH %s\n' "$DISK"
printf 'Confirmation: '
read -r CONFIRM
[ "$CONFIRM" = "FLASH $DISK" ] || { printf 'Confirmation did not match; nothing written.\n' >&2; exit 1; }

diskutil unmountDisk "$DISK"
RAW_DISK="/dev/r${DISK#/dev/}"
dd if="$IMAGE" of="$RAW_DISK" bs=4m
sync
diskutil eject "$DISK"
printf 'Write complete and disk ejected.\n'
