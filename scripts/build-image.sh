#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$SCRIPT_DIR/.." && pwd)
SDK_DIR=${DUO_SDK_DIR:-/tmp/music-lab-controller-duo-sdk}
OUT_DIR=${DUO_IMAGE_OUT:-$ROOT/build/image}
SDK_REF=v1.1.4
EXPECTED_REF=8e970aa49decfddc6f7f5bd66d0e798ffcf712c4

if [ ! -d "$SDK_DIR/.git" ]; then
    printf 'SDK missing; run scripts/build-duo-stock.sh first.\n' >&2
    exit 1
fi
if [ "$(git -C "$SDK_DIR" rev-parse HEAD)" != "$EXPECTED_REF" ]; then
    printf 'SDK revision mismatch; expected %s.\n' "$EXPECTED_REF" >&2
    exit 1
fi

BOARD_DIR=$SDK_DIR/device/milkv-duo-sd
KERNEL_DEFCONFIG=$SDK_DIR/build/boards/cv180x/cv1800b_milkv_duo_sd/linux/cvitek_cv1800b_milkv_duo_sd_defconfig
MARKER='# MUSIC_LAB_CONTROLLER_USB_HOST_FRAGMENT'

if [ ! -f "$KERNEL_DEFCONFIG" ]; then
    printf 'Expected kernel defconfig not found: %s\n' "$KERNEL_DEFCONFIG" >&2
    exit 1
fi

if ! grep -F "$MARKER" "$KERNEL_DEFCONFIG" >/dev/null 2>&1; then
    printf '\n%s\n' "$MARKER" >>"$KERNEL_DEFCONFIG"
    cat "$ROOT/buildroot/configs/kernel-usb-host.fragment" >>"$KERNEL_DEFCONFIG"
fi

mkdir -p "$BOARD_DIR/overlay" "$OUT_DIR"
cp -R "$ROOT/buildroot/overlay/." "$BOARD_DIR/overlay/"

# The application binaries are injected only into the external SDK workspace.
# Use the already-proven static RISC-V compiler path, or provide LAB_CC.
CC=${LAB_CC:-}
if [ -z "$CC" ]; then
    printf '%s\n' 'LAB_CC must name a RISC-V Linux C compiler for labd/lab-client.' >&2
    printf '%s\n' 'The official Milk-V x86_64 toolchain cannot run on ARM64 daisies.' >&2
    exit 1
fi
mkdir -p "$BOARD_DIR/overlay/usr/local/lib/music-lab" "$BOARD_DIR/overlay/usr/local/bin"
$CC -std=c99 -Os -Wall -Wextra -Werror -static \
    "$ROOT/labd/labd.c" -o "$BOARD_DIR/overlay/usr/local/lib/music-lab/labd"
$CC -std=c99 -Os -Wall -Wextra -Werror -static \
    "$ROOT/lab/lab-client.c" -o "$BOARD_DIR/overlay/usr/local/lib/music-lab/lab-client"
cp "$ROOT/lab/lab" "$BOARD_DIR/overlay/usr/local/bin/lab"
cp "$ROOT/tui/lab-tui" "$BOARD_DIR/overlay/usr/local/bin/lab-tui"
chmod 755 "$BOARD_DIR/overlay/usr/local/lib/music-lab/labd" \
    "$BOARD_DIR/overlay/usr/local/lib/music-lab/lab-client" \
    "$BOARD_DIR/overlay/usr/local/bin/lab" \
    "$BOARD_DIR/overlay/usr/local/bin/lab-tui"

cd "$SDK_DIR"
./build.sh milkv-duo-sd
image=$(find "$SDK_DIR/out" -maxdepth 1 -type f -name '*.img' -print | sort | tail -1)
if [ -z "$image" ]; then
    printf '%s\n' 'Custom build completed but no SD image was found.' >&2
    exit 1
fi
cp "$image" "$OUT_DIR/music-lab-duo.img"
printf 'Custom image: %s\n' "$OUT_DIR/music-lab-duo.img"
ls -lh "$OUT_DIR/music-lab-duo.img"
