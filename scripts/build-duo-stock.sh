#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$SCRIPT_DIR/.." && pwd)
SDK_URL=${DUO_SDK_URL:-https://github.com/milkv-duo/duo-buildroot-sdk.git}
SDK_REF=${DUO_SDK_REF:-v1.1.4}
SDK_DIR=${DUO_SDK_DIR:-/tmp/music-lab-controller-duo-sdk}

if [ "$(uname -s)" != Linux ]; then
    printf '%s\n' 'The official Milk-V SDK build must run on Linux.' >&2
    exit 1
fi

if [ ! -d "$SDK_DIR/.git" ]; then
    git clone --depth 1 --branch "$SDK_REF" "$SDK_URL" "$SDK_DIR"
else
    actual_ref=$(git -C "$SDK_DIR" rev-parse HEAD)
    expected_ref=8e970aa49decfddc6f7f5bd66d0e798ffcf712c4
    if [ "$actual_ref" != "$expected_ref" ]; then
        printf 'SDK at %s is %s, expected %s. Refusing to build.\n' \
            "$SDK_DIR" "$actual_ref" "$expected_ref" >&2
        exit 1
    fi
fi

actual_ref=$(git -C "$SDK_DIR" rev-parse HEAD)
printf 'Building official original-Duo target from %s (%s)\n' "$SDK_REF" "$actual_ref"
printf 'Command: (cd %s && ./build.sh milkv-duo-sd)\n' "$SDK_DIR"

cd "$SDK_DIR"
./build.sh milkv-duo-sd

image=$(find "$SDK_DIR/out" -maxdepth 1 -type f -name '*.img' -print | sort | tail -1)
if [ -z "$image" ]; then
    printf '%s\n' 'Build completed but no SD image was found under SDK/out.' >&2
    exit 1
fi

printf 'Stock image: %s\n' "$image"
ls -lh "$image"
