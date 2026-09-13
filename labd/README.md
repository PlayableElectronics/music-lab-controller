# labd

Future persistent control daemon. It will own logical device discovery,
transport sessions, routing, monitoring, and state independently of SSH/TUI
clients.

Milestone 1 intentionally contains no daemon implementation. First inspect the
stock Duo's USB, UART, and network capabilities with `scripts/probe-duo.sh`.
