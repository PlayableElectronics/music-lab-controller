#!/bin/sh
set -eu

printf '%s\n' 'Music Lab Controller image build scaffolding'
command -v git >/dev/null 2>&1 || { printf 'Missing prerequisite: git\n' >&2; exit 1; }
command -v make >/dev/null 2>&1 || { printf 'Missing prerequisite: make\n' >&2; exit 1; }
printf '%s\n' 'No Buildroot build was attempted.'
printf '%s\n' 'TODO: pin and verify the official Milk-V Duo 64 MB SDK/tag and image command.'
