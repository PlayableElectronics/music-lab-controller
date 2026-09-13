#!/bin/sh
set -eu

DUO_HOST="${DUO_HOST:-192.168.42.1}"
DUO_USER="${DUO_USER:-root}"

echo "Connecting to ${DUO_USER}@${DUO_HOST}"
exec ssh "${DUO_USER}@${DUO_HOST}" "$@"
