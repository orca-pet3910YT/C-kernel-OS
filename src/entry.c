#include <globals.h>
#include <vga.h>
#include <kb.h>
#include <string.h>
#include <panic.h>
#include <stdlib.h>
#include <serial.h>
#include <power.h>
#include <port.h>

void pic_remap() {
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x00);
	outb(0xA1, 0x00);
}

void kmain(void) {
	//volatile char* video = (volatile char*)0xB8000;
	//video[0] = 'E';
	//video[1] = 0x07;
	//putc('h');
	//putc('\n');
	//putc('i');
	//char string_thing[12] = {0};
	//ftoa(0.25, string_thing);
	//puts(string_thing);
	serial_init();
	printk(ver);
	printk("Initialized serial at 0x3F8 (COM1)");
	__asm__ volatile ("cli");
	printk("Clear interrupts");
	kb_init();
	printk("Initialized PS/2 BIOS keyboard");
	pic_remap();
	printk("Remapped the PIC");
	//__asm__ volatile ("sti");
	//printk("Interrupts set to on");
	printk("Hello, World!");
	puts(logo); // globals.h:4
	printk("Welcome to CkOS!");
	//putc(scancode_to_c(kb_get_scancode()));
	//panic("This PC is ass."); // compile with this uncommented to prank people :)
	char command[256] = {0};
	int index = 0;
	puts("$ ");
	for (;;) {
		char c = loop_until_keypress();
		putc(c);
		if (c == '\b') {
			if (index > 0) {
				index--;
				command[index] = '\0';
			}
			continue;
		}
		if (c != '\n') {
			command[index++] = c;
			command[index] = '\0';
		}
		if (c == '\n') {
			if (strcmp(command, "help") == 0) {
				puts("The commands are:\n"
				"hello: say hello to the world\n"
				"poweroff: turn the system off (QEMU only)\n"
				"reboot: restart the system\n"
				"halt: halt the CPU putting the entire system to a freeze\n"
				"help: display this message\n"
				"logo: display the logo\n"
				"ver: display the version\n");
			} else if (strcmp(command, "hello") == 0) {
				puts("Hello, World!\n");
			} else if (strcmp(command, "poweroff") == 0) {
				poweroff();
				panic("Failed to power off; likely not a QEMU machine");
			} else if (strcmp(command, "reboot") == 0) {
				reboot();
				panic("Failed to reboot; unknown error");
			} else if (strcmp(command, "halt") == 0) {
				while (1) halt();
			} else if (strcmp(command, "logo") == 0) {
				puts(logo);
			} else if (strcmp(command, "ver") == 0) {
				printk(ver);
			} else if (index > 0) {
				puts("Invalid command: ");
				puts(command);
				putc('\n');
			}
			index = 0;
			command[0] = '\0';
			puts("$ ");
			continue;
		}
	}
}
