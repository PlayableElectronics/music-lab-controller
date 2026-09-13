# labd

Future persistent control daemon. It will own logical device discovery,
transport sessions, routing, monitoring, and state independently of SSH/TUI
clients.

The daemon listens on `/tmp/labd.sock`, serves a small human-readable protocol,
and refreshes `/sys/class/tty` every two seconds. It has no threads, database,
JSON dependency, or UI dependency. `S70labd` starts it from BusyBox init and
stores only a small PID file under `/var/run`.

Build from a host with a RISC-V Linux/musl compiler using
`../scripts/build.sh`; the running stock Duo has no compiler. Install and
restart it with `../scripts/install.sh`.
