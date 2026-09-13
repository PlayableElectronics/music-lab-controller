#!/bin/sh
set -eu

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 IMAGE.img /dev/diskN" >&2
    echo "Example: $0 milkv-duo.img /dev/disk4" >&2
    exit 2
fi

IMAGE="$1"
DISK="$2"

case "$DISK" in
    /dev/disk[0-9]*) ;;
    *) echo "Refusing unexpected disk path: $DISK" >&2; exit 2 ;;
esac

if [ ! -f "$IMAGE" ]; then
    echo "Image not found: $IMAGE" >&2
    exit 2
fi

echo "WARNING: this will overwrite ALL DATA on $DISK"
diskutil info "$DISK" || exit 1
printf "Type the exact disk path (%s) to continue: " "$DISK"
read CONFIRM
[ "$CONFIRM" = "$DISK" ] || { echo "Cancelled."; exit 1; }

diskutil unmountDisk "$DISK"
RAW="$(printf '%s' "$DISK" | sed 's#/dev/disk#/dev/rdisk#')"
echo "Writing $IMAGE to $RAW ..."
sudo dd if="$IMAGE" of="$RAW" bs=4m status=progress
sync
diskutil eject "$DISK"
echo "Done."
