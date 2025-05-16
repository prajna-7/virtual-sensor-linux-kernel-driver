#!/bin/bash
qemu-system-x86_64 \
    -kernel bzImage \
    -initrd rootfs.cpio.gz \
    -append "console=ttyS0" \
    -nographic
