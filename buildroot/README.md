# Music Lab Duo image layer

This directory contains only project-owned configuration and overlay files.
The official Milk-V SDK is fetched outside this repository by the build
scripts.

Target: original Milk-V Duo / CV1800B / 64 MB / SD card / RISC-V.

Pinned base:

```text
Repository: https://github.com/milkv-duo/duo-buildroot-sdk.git
SDK tag:    v1.1.4
SDK commit: 8e970aa49decfddc6f7f5bd66d0e798ffcf712c4
Target:     milkv-duo-sd
Buildroot:  2021.05
```

Run `scripts/build-duo-stock.sh` on a compatible Linux build host first.
Then use `scripts/build-image.sh` to apply this layer to an external SDK
working copy. SDK and generated output directories are intentionally not
tracked here.

The first kernel fragment enables host-side USB network/serial support while
retaining the vendor DWC2 dual-role configuration. It is a customization
input, not a replacement kernel configuration.
