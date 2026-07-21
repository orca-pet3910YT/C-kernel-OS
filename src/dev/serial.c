#include <arch/i386/port.h>
#include <stddef.h>
#include <stdint.h>
#include <drivers/uart/serial.h>
#include <sys/globals.h>
#include <stdlib.h>
#include <drivers/video/vga.h>
#include <generated/config.h>
static unsigned short COM1 = 0x03F8;
static unsigned short COM2 = 0x02F8;
unsigned short UART1 = 0;
unsigned short UART2 = 0;

#ifdef CONFIG_SERIAL
#if CONFIG_SERIAL == 1
static void __init_com_(unsigned short port) {
	outb(port+1, 0x00);
	outb(port+3, 0x80);
	outb(port+0, 0x01);
	outb(port+1, 0x00); // 0x00
	outb(port+3, 0x03);
	outb(port+1, 0x01);
	outb(port+2, 0xC7); // 0xC7
	outb(port+4, 0x0B);
}
#endif
#endif

void serial_init() {
#ifdef CONFIG_SERIAL
#if CONFIG_SERIAL
		if (*(unsigned short *)0x400) {
			COM1 = *(unsigned short *)0x400;
			UART1 = COM1;
		} else {
			serial_com1 = 0;
		}
		if (*(unsigned short *)0x402) {
			COM2 = *(unsigned short *)0x402;
			UART2 = COM2;
		} else {
			serial_com1 = 0;
		}
		if (CONFIG_COM1 && UART1) {
			__init_com_(COM1);
			printk(5, "serial: Initialized COM1");
			serial_com1 = !(!UART1);
		}
		if (CONFIG_COM2 && UART2) {
			__init_com_(COM2);
			printk(5, "serial: Initialized COM2");
			serial_com2 = !(!UART2);
		}
#else
		printk(4, "serial: not configured because support is disabled");
#endif
#endif /* CONFIG_SERIAL */
}

void serial_shutdown() {
	serial_out = false;
	serial_in = false;
	outb(COM1+1, 0x00);
	outb(COM2+1, 0x00);
	printk(5, "serial: Serial shut down");
}

int transmit_fifo_empty_com1() {
	return inb(COM1+5) & 0x20;
}

int transmit_fifo_empty_com2() {
	return inb(COM2+5) & 0x20;
}

void sputc(char c) {
	if (c == '\n') sputc('\r');
	if (serial_com1) {
		while (!transmit_fifo_empty_com1());
		outb(COM1, c);
	}
	if (serial_com2) {
		while (!transmit_fifo_empty_com2());
		outb(COM2, c);
	}
}

// same story as puts, but for serial
void sputs(const char *s) {
	for (size_t i = 0; s[i] != '\0'; i++) {
		sputc(s[i]);
	}
}

/*char sgetc_raw() {
	//if (inb(COM1+5) & 1) { return inb(COM1); }
	char c = 0;
	uint8_t state = inb(COM1+5);
	if (!(state & 1)) return 0;
	c = inb(COM1);
	if (state & 0x1E) return 0;
	return c;
}*/

int sgetc_raw(unsigned char port) {
	unsigned short com = 0;
	if (serial_com1 && serial_com2) {
		com = port == 1 ? UART2 : UART1;
	} else if (serial_com1 && !serial_com2) {
		com = UART1;
	} else if (!serial_com1 && serial_com2) {
		com = UART2;
	} else {
		return 0;
	}
	uint8_t state = inb(com+5);
	if (!(state & 1)) return -1;
	if (state & 0x1E) {
		inb(com);
		return -1;
	}
	unsigned char c = inb(com);
	if (!c) return 0;
	return c;
}

char sgetc(unsigned char port) {
	char c = sgetc_raw(port);
	if (c < 0) return 0;
	if (c == '\r') {
		return '\n';
	} else if (c == 0x7F) {
		sputc('\b');
		sputc(' ');
		//sputc('\b');
		return '\b';
	}
	return c;
}
