#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$SCRIPT_DIR/.." && pwd)
LAB_HOST=${LAB_HOST:-root@192.168.42.1}
REMOTE_DIR=/tmp/music-lab-controller.$$

if ! "$SCRIPT_DIR/build.sh"; then
    printf 'Build failed; nothing was installed.\n' >&2
    exit 1
fi
mkdir -p "$ROOT/build"

cleanup() {
    ssh "$LAB_HOST" "rm -rf '$REMOTE_DIR'" >/dev/null 2>&1 || true
}
trap cleanup EXIT INT TERM

if ! ssh "$LAB_HOST" "mkdir -p '$REMOTE_DIR'"; then
    printf 'SSH connection failed.\n' >&2
    exit 1
fi
if ! scp -O "$ROOT/build/labd" "$ROOT/build/lab-client" "$ROOT/lab/lab" "$ROOT/tui/lab-tui" "$ROOT/image/overlay/etc/init.d/S70labd" "$LAB_HOST:$REMOTE_DIR/"; then
    printf 'SCP failed; nothing was installed.\n' >&2
    exit 1
fi
if ! ssh "$LAB_HOST" "mkdir -p /usr/local/lib/music-lab /usr/local/bin /etc/music-lab; cp '$REMOTE_DIR/labd' /usr/local/lib/music-lab/labd; cp '$REMOTE_DIR/lab-client' /usr/local/lib/music-lab/lab-client; cp '$REMOTE_DIR/lab' /usr/local/bin/lab; cp '$REMOTE_DIR/lab-tui' /usr/local/bin/lab-tui; chmod 755 /usr/local/lib/music-lab/labd /usr/local/lib/music-lab/lab-client /usr/local/bin/lab /usr/local/bin/lab-tui; ln -sf /usr/local/bin/lab /usr/bin/lab; if [ ! -f /etc/music-lab/devices.conf ]; then : > /etc/music-lab/devices.conf; fi; if [ -f /etc/init.d/S70labd ]; then /etc/init.d/S70labd stop >/dev/null 2>&1 || true; fi; cp '$REMOTE_DIR/S70labd' /etc/init.d/S70labd 2>/dev/null || true; chmod 755 /etc/init.d/S70labd 2>/dev/null || true; /usr/local/lib/music-lab/labd >/tmp/labd.log 2>&1 & echo \$! > /var/run/labd.pid; /usr/bin/lab ping; /usr/bin/lab status; /usr/bin/lab devices"; then
    printf 'Remote installation or verification failed.\n' >&2
    exit 1
fi
