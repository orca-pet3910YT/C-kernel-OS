#include <port.h>
#include <stddef.h>
#include <stdint.h>
#include <serial.h>
#include <globals.h>
#include <stdlib.h>
#define COM1 0x3F8

void serial_init() {
	outb(COM1+1, 0x00);
	outb(COM1+3, 0x80);
	outb(COM1+0, 0x01);
	outb(COM1+1, 0x00);
	outb(COM1+3, 0x03);
	outb(COM1+2, 0xC7); // 0xC7
	outb(COM1+4, 0x0B);
}

void serial_shutdown() {
	serial_out = false;
	outb(COM1+1, 0x00);
}

int transmit_fifo_empty() {
	return inb(COM1+5) & 0x20;
}

void sputc(char c) {
	while (!transmit_fifo_empty());
	outb(COM1, c);
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

int sgetc_raw() {
	uint8_t state = inb(COM1+5);
	if (!(state & 1)) return -1;
	if (state & 0x1E) {
		inb(COM1);
		return -1;
	}
	return inb(COM1);
}

char sgetc() {
	char c = sgetc_raw();
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
