#!/bin/sh
set -eu

HOST=${1:-192.168.42.1}
exec ssh "root@${HOST}"
