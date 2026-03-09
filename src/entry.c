#include <globals.h>
#include <vga.h>
#include <kb.h>
#include <string.h>
#include <panic.h>
#include <stdlib.h>
#include <serial.h>
#include <power.h>
#include <port.h>
#include <multiboot.h>
#include <idt.h>
#include <gdt.h>

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

char *__split_cmdline(char **buffer) {
	char *a, *b;
	a = *buffer;
	if (!a) return NULL;
	for (b = a; *b && strchr(b, ' '); b++);
	if (*b) {
		*b++ = '\0';
		*buffer = b;
	} else {
		*buffer = NULL;
	}
	return a;
}

void parse_cmdline(char *input) {
	char *a; char *b = input;
	while ((a = __split_cmdline(&b))) {
		if (*a == '\0') continue;
		printk("Received command line argument: %s", a);
		if (strcmp(a, "s_in") == 0) {
			serial_in = true;
			printk("Serial input enabled");
		} else if (strcmp(a, "s_out") == 0) {
			serial_out = true;
			printk("Serial output enabled");
		} else if (strcmp(a, "inton") == 0) {
			__asm__ volatile ("sti");
			printk("Interrupts enabled");
		} else {
			printk("Invalid command line argument %s, ignoring", a);
		}
	}
}

void kmain(int magic, mbinfo_t *mbi) {
	//volatile char* video = (volatile char*)0xB8000;
	//video[0] = 'E';
	//video[1] = 0x07;
	//putc('h');
	//putc('\n');
	//putc('i');
	//char string_thing[12] = {0};
	//ftoa(0.25, string_thing);
	//puts(string_thing);
	clear_screen();
	printk(ver);
	printk("Refreshed VGA");
	serial_init();
	printk("Initialized serial at 0x3F8 (COM1)");
        printk("Multiboot flags: %x", mbi->flags);
	char *cmdline = NULL;
	//char *strings[16];
	// FIXME: magic is 0
	//if (magic != 0x1BADB002) panic("Incorrect Multiboot 1 magic number! Got 0x%x, should be 0x1BADB002");
	__asm__ volatile ("cli");
	printk("Clear interrupts");
	kb_init();
	printk("Initialized PS/2 BIOS keyboard");
	gdt_init();
	printk("GDT initialized, have fun suffering, dev of this cursed code");
	pic_remap();
	printk("Remapped the PIC");
	init_idt();
	printk("IDT initialized");
	printk("Be careful what kind of shit can happen now!");
	printk("---BEGIN Command line info---");
	if (mbi->flags & (1 << 2)) {
		cmdline = (char*)mbi -> cmdline;
		printk("Command line: %s", cmdline);
	}
	parse_cmdline(cmdline);
	printk("Parsed command line provided by bootloader");
	printk("--- END Command line info ---");
	//unsigned int strn = (unsigned int)split(cmdline, ' ', strings, 15);
	//for (unsigned int i = 0; /*i < (sizeof(strings)/sizeof(strings[0]))*/ i < strn; i++) {
	//	printk("cmdline of %x: %s", i, strings[i]);
	//}
	parse_cmdline(cmdline);
	//__asm__ volatile ("sti");
	//printk("Interrupts set to on");
	printk("Hello, World!");
	set_color(0x0F);
	printf("%s\n", logo); // globals.h:4
	set_color(0x07);
	printk("Welcome to CkOS!");
	//putc(scancode_to_c(kb_get_scancode()));
	//panic("This PC is ass."); // compile with this uncommented to prank people :)
	char command[256] = {0};
	int index = 0;
	printf("$ ");
	for (;;) {
		//char c = loop_until_keypress();
		char c = kbc; // from globals
		if (c) { putc(c); } else { continue; }
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
				printf("The commands are:\n"
				"hello: say hello to the world\n"
				"poweroff: turn the system off (QEMU only)\n"
				"reboot: restart the system\n"
				"halt: halt the CPU putting the entire system to a freeze\n"
				"help: display this message\n"
				"logo: display the logo\n"
				"ver: display the version\n"
				"clear: clear the screen\n"
				"panictest: test the panic functionality\n");
			} else if (strcmp(command, "hello") == 0) {
				printf("Hello, World!\n");
			} else if (strcmp(command, "poweroff") == 0) {
				poweroff();
				panic("Failed to power off; likely not a QEMU machine");
			} else if (strcmp(command, "reboot") == 0) {
				reboot();
				panic("Failed to reboot; unknown error");
			} else if (strcmp(command, "halt") == 0) {
				printf("System halted. It is now safe to power off.\n");
				while (1) halt();
			} else if (strcmp(command, "logo") == 0) {
				set_color(0x0F);
				printf("%s\n", logo);
				set_color(0x07);
			} else if (strcmp(command, "ver") == 0) {
				printf("%s\n", ver);
			} else if (strcmp(command, "panictest") == 0) {
				panic("User-triggered panic");
			} else if (strcmp(command, "clear") == 0) {
				clear_screen();
			} else if (strcmp(command, "exit") == 0) {
				return;
			} else if (strcmp(command, "credits") == 0) {
				printf(credits);
			} else if (index > 0) {
				printf("Invalid command: %s\n", command);
			}
			index = 0;
			command[0] = '\0';
			printf("$ ");
			continue;
		}
	}
}
