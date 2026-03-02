#include <vga.h>
#include <kb.h>
#include <string.h>
#include <panic.h>

void kmain(void) {
	//volatile char* video = (volatile char*)0xB8000;
	//video[0] = 'E';
	//video[1] = 0x07;
	//putc('h');
	//putc('\n');
	//putc('i');
	puts("[ 0.000000] Hello, World!\n");
	puts("[ 0.000000] CkOS Version 0.01 for i386 (x86-32)\n");
	//putc(scancode_to_c(kb_get_scancode()));
	//panic("This PC is ass."); // compile with this uncommented to prank people :)
	char command[256] = {0};
	int index = 0;
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
		}
	}
}
