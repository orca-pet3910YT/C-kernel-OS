#include <port.h>
#include <stdint.h>
#include <vga.h>
#include <power.h>

static inline void outw(uint16_t port, uint16_t value) {
	__asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

int poweroff() {
	printk("power: Powering off");
	outw(0x604, 0x2000); // new QEMU
	outw(0xB004, 0x2000); // bochs and old QEMU
	outw(0x4004, 0x3400); // virtualbox (ew)
	outw(0x600, 0x34); // Cloud Hypervisor
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
