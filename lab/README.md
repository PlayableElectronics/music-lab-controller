# lab CLI

Future small command-line client for `labd`. It should use a human-readable
line protocol so commands can also be tested with BusyBox tools such as
`echo`, `cat`, `screen`, `minicom`, or `socat`.

Example future protocol:

```text
@FPGA GET STATUS
@FPGA SET FREQ 110
@FPGA STRIKE
```
