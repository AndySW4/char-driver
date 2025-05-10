# pa6\_char\_driver

A simple **character device driver** (Loadable Kernel Module). It stores up to **900 bytes** in a kernel buffer and supports open/close, read, write, and seek.

```text
Major #: 511   Minor #: 0   Device file: /dev/pa6_char_device
Buffer : 900 bytes (in‑kernel RAM)
```

---

## Build

```bash
# Inside the directory that contains pa6_char_driver.c
make -C /lib/modules/$(uname -r)/build M="$(pwd)" modules
```

This invokes the kernel build system and produces **pa6\_char\_driver.ko**.

> If you do not have a Makefile yet, create one with:
>
> ```make
> obj-m += pa6_char_driver.o
> ```

---

## Install & create the device file

```bash
sudo insmod pa6_char_driver.ko          # load module
sudo mknod -m 777 /dev/pa6_char_device c 511 0
```

Check that it loaded:

```bash
lsmod | grep pa6_char_driver            # should list the module
cat /proc/devices | grep 511            # major number appears
```

Kernel messages (open/close/read/write/seek) are visible with:

```bash
dmesg --follow
```

---

## Usage examples

```bash
# Write and read via shell utilities
printf "hello world" > /dev/pa6_char_device      # write 11 bytes
head -c 5 /dev/pa6_char_device                   # read "hello"

echo "more" >> /dev/pa6_char_device              # append via seek‑to‑end
cat /dev/pa6_char_device                         # full buffer contents

# Seek to offset 0 then overwrite
dd if=/dev/zero of=/dev/pa6_char_device bs=1 count=1 seek=0
```

All operations log their byte counts to the kernel ring buffer.

---

## Uninstall

```bash
sudo rm /dev/pa6_char_device
sudo rmmod pa6_char_driver
```

---

## Notes

* Handles partial reads/writes at buffer boundaries.
* Seeks clamp to `[0, 900]`—no error returns.
* Tested on Ubuntu **6.5.0‑15‑generic** with GCC 12.

---
