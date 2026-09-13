# Image trimming

No vendor feature has been removed yet. The first custom image keeps the
official boot chain and platform support intact while adding the smallest
USB-host and music-lab layer. Camera, AI, multimedia, and sample components
will be measured from a successful reproducible build before any removal is
considered.

The stock image already omits many loadable modules from `/mnt/system/ko`, but
the kernel itself contains DWC2, USB storage, HID, and USB audio support. The
initial customization therefore adds only host-side serial and Ethernet
drivers needed for the controller milestone. Trimming is deferred until
before/after image sizes and boot behavior can be compared.
