# 2 - Kernel Core
The CkOS kernel consists of not much of a core. It's just as simple as it gets.

## Init
The kernel start is `_start()` in src/boot.s. It sets up the stack and calls `kmain()` from src/entry.c, and if the function somehow returns, `panic()` from include/panic.h is called.

### Stages
- `clear_screen()`
- `serial_init()`
- `parse_cmdline(cmdline)`
- `kb_init()`
- `gdt_init()`
- `pic_remap()`
- `init_idt()`
- `__asm__ volatile ("sti")`
- `init_pit()`

## How does the kernel embed a build date?
The Makefile runs a few scripts on build every time it is run.

The script in question is called gen_ver.sh. Its job is to fetch the user, hostname, run date and operating system type.

```sh
#!/bin/bash
date_bld=$(date)
echo "Generating include/generated/__GENVER.h"
mkdir -p include/generated
tr -d '\r' > include/generated/__GENVER.h <<EOF
#ifndef CKOS_BLD
#define CKOS_BLD "$(whoami)@$(hostname) at ${date_bld} on $(uname -s)"
#endif
EOF
```

The script manually generates a header file for C, or in our case, the C kernel.

This is almost exactly what Linux already does. You can see the header saying similar stuff (except the compiler in the provided script).

## Drivers
(See ./1-drivers.md)

## Shell Commands
hello: say hello to the world

poweroff: turn the system off (QEMU only)

reboot: restart the system

halt: halt the CPU putting the entire system to a freeze

help: display the help message

logo: display the logo

ver: display the version

clear: clear the screen

panictest: test the panic functionality
