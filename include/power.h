#pragma once
#include <port.h>
#include <stdint.h>
#include <vga.h>

static inline void outw(uint16_t port, uint16_t value) {
	__asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

int poweroff() {
	printk("power: Powering off");
	outw(0x604, 0x2000);
	return 1;
}

int reboot() {
	printk("power: Rebooting");
	outb(0x64, 0xFE);
	return 1;
}

void halt() {
	__asm__ volatile ("cli; hlt");
}
