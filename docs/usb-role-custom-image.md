# USB role in the custom image

The custom image will retain the vendor DWC2 dual-role controller in the
kernel and make host mode the intended normal role. DWC2 is built in by the
official original-Duo defconfig (`CONFIG_USB_DWC2=y`); no runtime `dwc2.ko`
is required.

The stock image’s `/etc/uhubon.sh host` is not suitable as a direct template:
it first tries to insert a missing `/mnt/system/ko/dwc2.ko`. The working
custom-image implementation must write the vendor role control only after the
custom kernel/device-tree behavior is validated. Gadget/device mode remains
the recovery role, controlled through the UART console.

No custom image has been flashed yet, so the exact boot-time role script and
host/hub result remain unverified. The known-good stock card is untouched.
