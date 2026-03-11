# 2 - Kernel Core
The CkOS kernel consists of not much of a core. It's just as simple as it gets.

## Init
The kernel start is `_start()` in src/boot.s. It sets up the stack and calls `kmain()` from src/entry.c, and if the function somehow returns, `panic()` from include/panic.h is called.

### Stages
- `clear_screen()`
- `serial_init()`
- `kb_init()`
- `gdt_init()`
- `pic_remap()`
- `init_idt()`
- `parse_cmdline(cmdline)`
- `__asm__ volatile ("sti")`
- `init_pit()`

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
