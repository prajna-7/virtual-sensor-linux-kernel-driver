# Virtual Sensor Driver - Complete Kernel Project

## Overview

This project implements a virtual temperature sensor as a Linux kernel module (character device). It demonstrates:

- Character device interaction via `/dev/virt_sensor`
- Sysfs interface to set `mode`
- Simulated memory leak detection
- Ready for QEMU simulation
- Placeholder for future Device Tree overlay usage

## Features

- Read simulated data from user space
- Dynamically configure mode via `/sys/kernel/virt_sensor/mode`
- Demonstrate debugging with `printk`, `dmesg`
- Simulate memory leak with `kmalloc`
- Cleanly free resources on module removal
- Bootable via QEMU with Buildroot rootfs

## Run Instructions

### 1. Build Module

```bash
cd src
make
sudo insmod virt_sensor.ko
```

### 2. Use from User-space

```bash
cd ../userspace
gcc read_sensor.c -o read_sensor
./read_sensor
```

### 3. Set Mode via Sysfs

```bash
echo 1 | sudo tee /sys/kernel/virt_sensor/mode
cat /sys/kernel/virt_sensor/mode
```

### 4. Remove Module

```bash
sudo rmmod virt_sensor
```

### 5. Run in QEMU

```bash
cd qemu
./run_qemu.sh
```

### 6. Detect Memory Leak (if kmemleak enabled)

```bash
echo scan > /sys/kernel/debug/kmemleak
cat /sys/kernel/debug/kmemleak
```

## Device Tree (DT) Placeholder

This module is written to be easily extended with a Device Tree overlay for real sensor attachment. You can simulate this in QEMU or Raspberry Pi by assigning the device node via DTS later.

## License

GPL


## Kmemleak Setup
To enable kmemleak in your kernel:

1. Configure the kernel with:

    CONFIG_DEBUG_KMEMLEAK=y
    CONFIG_DEBUG_KMEMLEAK_EARLY_LOG_SIZE=400
    CONFIG_DEBUG_KMEMLEAK_DEFAULT_ON=y

2. Recompile the kernel and boot with `kmemleak=on`:

    ```bash
    qemu-system-x86_64 -kernel bzImage -append "console=ttyS0 kmemleak=on"
    ```

3. Mount debugfs and use kmemleak:

    ```bash
    mount -t debugfs none /sys/kernel/debug
    echo scan > /sys/kernel/debug/kmemleak
    cat /sys/kernel/debug/kmemleak
    ```

4. Free the memory or patch the leak in your module code.
