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

## PIT
This is a rather boring driver. The clock is set by default to 1 kilohertz or 1000 hertz. You initialize this driver by calling `init_pit()`.

Before the PIT is initialized, clock behavior is undefined. Usually it's initialized to ~18.1 hertz, but it is not a reliable standard.

The clock is counted in not a very reliable way - the FPU is used every millisecond when the clock ticks, so it sometimes causes the double to be filled with 9s due to floating point error.

Likely this can be fixed by instead incrementing a global *tick* counter and calculating the time when printing.

## Graphics
This driver is one of the longest ones. There is multiple ways to print, like putc, puts, printf, print, printk etc.

Printing a single character requires putc to check for special characters like the newline, tab, backspace etc. That's because putting them directly in the VGA text buffer at **0xB8000** will print a control character.

The codepage used is cp737. You can search it online and find what exactly gets printed.

You need column and row variables which can't go higher than 80 and 25 respectively.
