# 2 - Kernel Core
The CkOS kernel consists of not much of a core. It's just as simple as it gets.

## Init
The kernel start is `_start()` in src/boot.s. It sets up the stack and calls `kmain()` from src/entry.c, and if the function somehow returns, `panic()` from include/panic.h is called.
