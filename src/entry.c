#include <vga.h>
#include <kb.h>
#include <string.h>
#include <panic.h>
#include <stdlib.h>
#include <serial.h>

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
	printk("Initialized serial at 0x3F8 (COM1)");
	//__asm__ volatile ("sti");
	//printk("Interrupts set to on");
	printk("Hello, World!");
	printk("CkOS Version 0.01 for i386 (x86-32)");
	const char* logo =
" __   _  _\n"
"/  |/| ||_`\n"
"\\__|\\|_|._|\n"
"\n\0";
	puts(logo);
	printk("Welcome to CkOS 0.01!");
	//putc(scancode_to_c(kb_get_scancode()));
	//panic("This PC is ass."); // compile with this uncommented to prank people :)
	char command[256] = {0};
	int index = 0;
	puts("$ ");
	for (;;) {
		char c = loop_until_keypress();
		putc(c);
		if (index == 255) {
			panic("user input exceeded limit of 255");
		}
		command[index++] = c;
		command[index] = '\0';
		if (strcmp(command, "test\n") == 0) {
			puts("Hello, World!\n");
			for (unsigned int i = 0; i < 256; i++) {
				command[i] = 0;
			}
			index = 0;
			puts("$ ");
		} else if (command[index-1] == '\n') {
			puts("Invalid command: ");
			puts(command);
			putc('\n');
			index = 0;
			for (unsigned int i = 0; i < 256; i++) {
				command[i] = 0;
			}
			puts("$ ");
		}
	}
}
