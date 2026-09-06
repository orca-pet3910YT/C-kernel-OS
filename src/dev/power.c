#include <arch/i386/port.h>
#include <stdint.h>
#include <drivers/video/vga.h>
#include <drivers/power/power.h>
#include <drivers/uart/serial.h>
#include <arch/i386/pic.h>
#include <drivers/timers/pit.h>
#include <fs/vfs.h>

void system_shutdown() {
	printk(4, "power: The system will shut down!");
	vfs_shutdown();
	printk(4, "Shut down VFS");
	__asm__ volatile ("cli");
        printk(6, "power: Disabled interrupts");
        serial_shutdown();
        printk(6, "Disabled serial");
        pic_shutdown();
        printk(6, "Disabled PIC");
        pit_shutdown();
        printk(6, "Disabled PIT");
        while (inb(0x64) & 0x02);
        outb(0x64, 0xAD);    
        printk(6, "Disabled PS/2 keyboard");
}

int poweroff() {
	system_shutdown();
	printk(4, "power: Powering off");
	outw(0x604, 0x2000); // new QEMU
	outw(0xB004, 0x2000); // bochs and old QEMU
	outw(0x4004, 0x3400); // virtualbox (ew)
	return 1;
}

int reboot() {
	system_shutdown();
	printk(4, "power: Rebooting");
	outb(0x64, 0xFE);
	return 1;
}

void halt() {
	system_shutdown();
	__asm__ volatile ("cli; hlt");
}
