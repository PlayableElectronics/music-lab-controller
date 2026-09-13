# Buildroot base

## Pinned official SDK

The project targets the original 64 MB CV1800B Duo SD board, not Duo 256M or
Duo S. The selected official source is:

```text
Repository: https://github.com/milkv-duo/duo-buildroot-sdk.git
Tag:        v1.1.4
Commit:     8e970aa49decfddc6f7f5bd66d0e798ffcf712c4
Target:     milkv-duo-sd
Board:      cv1800b_milkv_duo_sd
```

The tag contains Buildroot 2021.05, Linux 5.10 sources, U-Boot 2021.10,
OpenSBI, and the CV1800B 64 MB SD board configuration. The official SDK
README documents `./build.sh milkv-duo-sd` and places generated SD images in
the SDK `out/` directory. The project scripts fetch this SDK outside the
repository; the SDK itself is never vendored.

The official kernel repository is
`https://github.com/milkv-duo/milkv-duo-linux.git`, branch/tag
`linux-5.10.4` / `duo-linux-5.10.4`, currently resolving to
`4e24739a177e903f750ca5fad11fa501f3e540d1`. The live image’s vendor vermagic
contains `g3803f57def61`, which is not the public branch tip and cannot be
reliably mapped to a public commit from the running image alone. The custom
image must therefore build the kernel from the SDK’s own `linux_5.10` source,
not substitute a standalone upstream module.

The live golden image reports `Buildroot 2021.05`, `VERSION=20241122-1401`,
and Linux `5.10.4-tag-`. Its kernel boot log shows DWC2 registering the
`4340000.usb` controller and a root USB bus. The live image has no
`/proc/config.gz`; known-good modules report vermagic
`5.10.4-t4.1.0-g3803f57def61 preempt mod_unload riscv` and the Xuantie-900
musl GCC 10.2.0 toolchain.

## Build commands

On a compatible Linux host with the official SDK prerequisites:

```sh
./scripts/build-duo-stock.sh
LAB_CC=/path/to/riscv64-linux-musl-gcc ./scripts/build-image.sh
```

The current `daisies` host is ARM64, while the official Milk-V toolchain
bundled by this SDK is x86_64. This is a known build-host compatibility issue;
the scripts refuse to pretend that the incompatible compiler is usable. The
first stock-build attempt on 2026-09-13 also exhausted the Pi’s 5.7 GiB free
root storage during the SDK clone. The build script now refuses hosts with
less than 10 GiB free near the SDK directory. No image was produced.
