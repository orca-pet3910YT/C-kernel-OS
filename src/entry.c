#include <globals.h> // Globals
#include <vga.h> // Display characters and string
#include <kb.h> // Keyboard functions (deprecated outside IDT)
#include <string.h> // String tools
#include <panic.h> // Critical errors
#include <stdlib.h> // Number to string functions
#include <serial.h> // Serial I/O
#include <power.h> // Power options
#include <port.h> // Port I/O
#include <multiboot.h> // Multiboot info for command line
#include <idt.h> // Interrupt handling
#include <gdt.h> // Required for IDT
#include <pit.h> // Programmable Interval Timer
#include <pic.h> // PIC
#include <generated/__GENVER.h> // Build info
#include <cpu.h> // CPU vendor string
#include <generated/__BLD.h> // Build number

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
		// FIXME: serial input doesn't work!
		/*if (strcmp(a, "s_in") == 0) {
			serial_in = true;
			printk("Serial input enabled");
		} else*/ if (strcmp(a, "s_out") == 0) {
			serial_out = true;
			printk("Serial output enabled");
		} else {
			printk("Invalid command line argument %s, ignoring", a);
		}
	}
}

extern void _start();
void debug_info_print() {
	printk(ver); printk("Build #%d by %s", BUILD_NUMBER, CKOS_BLD);
	printk("Kernel entry offset: %x, image offset: %x", _start, 1024*1024);
	printk("Made by orca.pet3910YT with %s", "\x03");
}

void kmain(int magic, mbinfo_t *mbi) {
	(void)magic;
	//volatile char* video = (volatile char*)0xB8000;
	//video[0] = 'E';
	//video[1] = 0x07;
	//putc('h');
	//putc('\n');
	//putc('i');
	//char string_thing[12] = {0};
	//ftoa(0.25, string_thing);
	//puts(string_thing);
	debug_info_print();
	serial_init();
	printk("Initialized serial at 0x3F8 (COM1)");
	printk("Multiboot flags: %x", mbi->flags);
	char *cmdline = NULL;
	//char *strings[16];
	// FIXME: magic is 0
	//if (magic != 0x1BADB002) panic("Incorrect Multiboot 1 magic number! Got 0x%x, should be 0x1BADB002");
	if (mbi->flags & (1 << 2)) {
		cmdline = (char*)mbi -> cmdline;
		printk("Command line: %s", cmdline);
	}
	printk("---BEGIN Command line info---");
	parse_cmdline(cmdline);
	printk("Parsed command line provided by bootloader");
	printk("--- END Command line info ---");
	kb_init();
	printk("Initialized PS/2 BIOS keyboard");
	gdt_init();
	printk("GDT initialized");
	pic_remap();
	printk("Remapped the PIC");
	init_idt();
	printk("IDT initialized");
	//unsigned int strn = (unsigned int)split(cmdline, ' ', strings, 15);
	//for (unsigned int i = 0; /*i < (sizeof(strings)/sizeof(strings[0]))*/ i < strn; i++) {
	//	printk("cmdline of %x: %s", i, strings[i]);
	//}
	__asm__ volatile ("sti");
	printk("Set interrupts");
	init_pit();
	printk("Initialized PIT at 1.43 KHz");
	printk("isr6(): %x", isr6);
	printk("isr0(): %x", isr0);
	printk("CPU: %s", get_cpu_vendor());
	// get_cpu_vendor in cpu.asm is a simple assembly function that uses cpuid with eax = 0
	// which puts the max eax value in eax and the CPU vendor in ebx, edx and ecx in order.
	// technically i don't think you have to call it more than once but never too safe :P
	printk("Hello, World!");
	set_color(0x0F);
	printf("%s\n", logo); // globals.h:4
	set_color(0x07);
	printk("Welcome to CkOS!");
	//putc(scancode_to_c(kb_get_scancode()));
	//panic("This PC is ass."); // compile with this uncommented to prank people :)
	char command[256] = {0};
	int index = 0;
	//int lastchar = 0;
	printf("$ ");
	for (;;) {
		//char c = loop_until_keypress();
		char c = kbc; // from globals
		__asm__ volatile ("pause");
		if (c) { putc(c); kbc = 0; } else { __asm__ volatile ("hlt"); continue; }
		if (c == '\b') {
			if (index > 0) {
				index--;
				command[index] = '\0';
			}
			//lastchar--;
			continue;
		}
		if (c == 0x1A || c == 0x1B || c == 0x1E || c == 0x1F) { // all of these are arrow keys
			continue; // arrow handling is painful in our world
		}
		/*if (c == 0x1A) {
			if (index > 0) {
				index--;
			}
			continue;
		}*/
		if (c != '\n') {
			command[index++] = c;
			command[index] = '\0'; // lastchar
			//lastchar++;
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
				"panictest: test the panic functionality\n"
				"crash: triggers a crash\n"
				"cpuinfo: get CPU info\n");
			} else if (strcmp(command, "hello") == 0) {
				printf("Hello, World!\n");
			} else if (strcmp(command, "poweroff") == 0) {
				poweroff();
				panic("Failed to power off; likely not a QEMU machine");
			} else if (strcmp(command, "reboot") == 0) {
				reboot();
				__asm__ volatile ("hlt");
				panic("Failed to reboot; unknown error");
			} else if (strcmp(command, "halt") == 0) {
				printf("System halted. It is now safe to power off.\n");
				while (1) halt();
			} else if (strcmp(command, "waitint") == 0) {
				__asm__ volatile ("hlt");
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
			} else if (strcmp(command, "crash") == 0) {
				//__asm__ volatile ("int $0");
				__asm__ volatile ("int3");
			} else if (strcmp(command, "cpuinfo") == 0) {
				printk("CPU vendor: '%s', friendly name '%s'", get_cpu_vendor(), get_cpu_vendor_user());
			} else if (strcmp(command, "oopstest") == 0) {
				oops("User-triggered oops");
			} else if (index > 0) { // lastchar
				printf("Invalid command: %s\n", command);
			}
			index = 0;
			//lastchar = 0;
			command[0] = '\0';
			printf("$ ");
			continue;
		}
	}
}
