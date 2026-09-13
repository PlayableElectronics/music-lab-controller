#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
REPO_DIR=$(CDPATH= cd -- "$SCRIPT_DIR/.." && pwd)
LAB_HOST=${LAB_HOST:-root@192.168.42.1}
REMOTE_PATH=/tmp/music-lab-lab.$$

cleanup() {
    ssh "$LAB_HOST" "rm -f '$REMOTE_PATH'" >/dev/null 2>&1 || true
}
trap cleanup EXIT INT TERM

printf 'Installing lab to %s:/usr/local/bin/lab\n' "$LAB_HOST"
# The stock Buildroot SSH server does not provide /usr/libexec/sftp-server.
# Force OpenSSH's legacy SCP protocol instead of the macOS scp SFTP default.
if ! scp -O "$REPO_DIR/lab/lab" "$LAB_HOST:$REMOTE_PATH"; then
    printf 'SCP failed; lab was not installed.\n' >&2
    exit 1
fi

if ! ssh "$LAB_HOST" "mkdir -p /usr/local/bin && cp '$REMOTE_PATH' /usr/local/bin/lab && chmod 755 /usr/local/bin/lab && ln -sf /usr/local/bin/lab /usr/bin/lab && rm -f '$REMOTE_PATH' && /usr/bin/lab status"; then
    printf 'SSH installation or remote lab status failed.\n' >&2
    exit 1
fi
