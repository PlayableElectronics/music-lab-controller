#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
OUT=${LAB_BUILD_DIR:-$ROOT/build}
CC=${LAB_CC:-}

if [ -z "$CC" ]; then
    for candidate in riscv64-unknown-linux-musl-gcc riscv64-linux-musl-gcc riscv64-linux-gnu-gcc; do
        if command -v "$candidate" >/dev/null 2>&1; then
            CC=$candidate
            break
        fi
    done
fi

if [ -z "$CC" ]; then
    printf 'No RISC-V Linux compiler found.\n' >&2
    printf 'Set LAB_CC to a riscv64 Linux C compiler and rerun.\n' >&2
    printf 'The official Milk-V compiler is a Linux x86_64 executable; use it from a Linux x86_64 host or VM.\n' >&2
    exit 1
fi

mkdir -p "$OUT"
COMMON='-std=c99 -Os -Wall -Wextra -Werror'
# The stock Duo uses musl. Static linking avoids a runtime loader mismatch.
$CC $COMMON -static "$ROOT/labd/labd.c" -o "$OUT/labd"
$CC $COMMON -static "$ROOT/lab/lab-client.c" -o "$OUT/lab-client"
strip "$OUT/labd" "$OUT/lab-client" 2>/dev/null || true
printf 'Built %s and %s with %s\n' "$OUT/labd" "$OUT/lab-client" "$CC"
