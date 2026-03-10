#include "stdint.h"
#include "port.h"

uint8_t inb(uint16_t port) {
	uint8_t data;
	__asm__ volatile ("inb %1, %0" : "=a" (data) : "dN" (port));
	return data;
}

void outb(uint16_t port, uint8_t data) {
	__asm__ volatile ("outb %0, %1" : : "a" (data), "dN" (port));
}

void wait_port() {
	__asm__ volatile ("outb %%al, $0x80" : : "a"(0));
}
