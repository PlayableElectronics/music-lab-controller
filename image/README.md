# Image scaffolding

This directory will hold project configuration, an overlay, and patches for
the official Milk-V Duo Buildroot/SDK flow. The full Milk-V SDK is deliberately
not vendored here.

The target is the original CV1800B / 64 MB Duo. Before implementing a fetch or
build wrapper, pin an upstream SDK tag and verify its official image output and
invocation against the stock board. Until then, `build.sh` only validates
local prerequisites and reports the missing integration work.
