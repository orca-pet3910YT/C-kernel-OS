# 1 - Drivers
There isn't much to this OS and drivers. After all this is below 2k lines.

But the driver list is as follows:
- kb: Keyboard
- pit: PIT
- vga: Graphics
- power: Power modes
- serial: Serial I/O
- port: Raw PMIO

## Keyboard
The keyboard driver is crazy. This is for PS/2, but CSM turns a USB keyboard to PS/2 I/O.

You initialize this driver by calling `kb_init()`. I doubt this does much because I forgot
to initialize it in really early 0.01.

You have to get the raw scancodes by checking if `inb(0x64) & 1` is truthy.
Then, you read a byte from `inb(0x60)`. Even then, you only get scancodes.

It is up to you how to handle the scancode in the kernel. It gives you when the key is pressed
or released.
