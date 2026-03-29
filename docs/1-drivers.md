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
<details>
<summary>Click for details</summary>

The keyboard driver is crazy. This is for PS/2, but CSM turns a USB keyboard to PS/2 I/O.

You initialize this driver by calling `kb_init()`. I doubt this does much because I forgot
to initialize it in really early 0.01.

You have to get the raw scancodes by checking if `inb(0x64) & 1` is truthy.
Then, you read a byte from `inb(0x60)`. Even then, you only get scancodes.

It is up to you how to handle the scancode in the kernel. It gives you data on when the key is pressed
or released.
</details>

## PIT
<details>
<summary>Click for details</summary>

This is a rather boring driver. The clock is set by default to 1.43 kilohertz or 1429 hertz. You initialize this driver by calling `init_pit()`.

Before the PIT is initialized, clock behavior is undefined. Usually it's initialized to ~18.1 hertz, but it is not a reliable standard.

The clock is counted in not a very reliable way - the FPU is used every millisecond when the clock ticks, so it sometimes causes the double to be filled with 9s due to floating point error.

Likely this can be fixed by instead incrementing a global *tick* counter and calculating the time when printing.
</details>

## Graphics
<details>
<summary>Click for details</summary>

This driver is one of the longest ones. There is multiple ways to print, like putc, puts, printf, print, printk etc.

A typical 80x25 screen is made of 2000 cells or 4000 bytes. A cell has a high byte (the color) and low byte (the character). The VGA colors are the standard 16 color RGB palette.

The color byte's left hex digit is the background color and the right digit is the foreground. Usually you want 0x07 because that's the typical color used in bootloaders.

Printing a single character requires putc to check for special characters like the newline, tab, backspace etc. That's because putting them directly in the VGA text buffer at **0xB8000** will print a control character.

The codepage used is cp737. You can search it online and find what exactly gets printed.

You need column and row variables which can't go higher than 80 and 25 respectively.
These are then set to the hardware cursor position by sending 4 bytes to ports 0x3D4 and 0x3D5.

As of CkOS 0.02, a heart is printed by leveraging Codepage 737. That's achieved with 0x03, which in modern terminal emulators just prints nothing or a gibberish character. Those use UTF-8 with regular ANSI instead.
</details>

## Power
<details>
<summary>Click for details</summary>

This is the least driver-like part of this section. It contains self-explanatory functions: `poweroff()` (QEMU only), `reboot()` and `halt()`.
</details>

## Serial
<details>
<summary>Click for details</summary>
  
This is like VGA but much simpler - you just get the baud right. In this kernel it's 115200 baud. You use `sputc()` and `sgetc()` for printing and receiving characters on serial.

Serial is initialized with `serial_init()`. The divisor determines the final baud, which is 0x01 for the specified above 115200 baud.

The main reason why CkOS 0.02 doesn't support serial input is because input methods are being shifted into interrupt-based input instead of poll-based methods.

Fun fact, it is not actually bad design to `printk()` to VGA even in serial-only mode, at least not until device detection phases are implemented. This is because the VGA text buffer is just physical memory. Nothing bad happens if you just write data there.
</details>

## Port
<details>
<summary>Click for details</summary>
  
Again, not much to talk about - you use `outb()` to output a byte and `inb()` to read a byte from a specified port.

Those functions translate into inline assembly with the actual instructions, `outb` and `inb`.
</details>
