# CkOS
This is CkOS, which stands for C kernel OS.

The features in this OS are... pretty lacking, but that'll be fixed over time.

[![Compile CkOS](https://github.com/orca-pet3910YT/C-kernel-OS/actions/workflows/compile.yml/badge.svg)](https://github.com/orca-pet3910YT/C-kernel-OS/actions/workflows/compile.yml)

## Build requirements (Debian apt package names, check for your distro)
- `clang`
- `binutils-i686-linux-gcc`
- `make`
- `nasm`
- `xorriso`
- `grub-common`
- `grub-pc-bin`
- `kconfig-frontends`

## Actually building
To build this OS, you'll need build dependencies from above.

The Make targets are:

- `all`: just build the kernel
- `run`: run the OS (and build the kernel if not already)
- `run-vnc`: run the OS with graphics on the VNC server display :0 (previously used for developing on Android by the owner)
- `run-debug`: run the OS with a GDB server on port :1234
- `clean`: clean build files (not necessary for development)
- `mrproper`: clean up EVERYTHING
- `iso`: build the ISO
- `isogz`: build and compress the ISO
- `menuconfig`: configure the kernel
- `allyesconfig`: enable everything
- `allnoconfig`: disable everything

## Other projects worth visiting

- [aOS - a bare metal OS from scratch by @axrxvm](https://github.com/axrxvm/aos)
- [CedarOS - another bare metal OS with a custom bootloader by @CedarDude](https://github.com/CedarDude/Cedar-OS)
- [kernel-OS - my previous attempt at an assembly kernel](https://github.com/orca-pet3910YT/kernel-OS)
