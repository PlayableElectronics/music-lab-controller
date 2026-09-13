# lab CLI

Future small command-line client for `labd`. It should use a human-readable
line protocol so commands can also be tested with BusyBox tools such as
`echo`, `cat`, `screen`, `minicom`, or `socat`.

Current client commands:

```sh
lab ping
lab status
lab devices
lab device NAME
lab refresh
lab send NAME TEXT...
lab monitor NAME
lab host-status
```

The daemon protocol is intentionally line-oriented. Example future device
protocol:

```text
@FPGA GET STATUS
@FPGA SET FREQ 110
@FPGA STRIKE
```
