#include <vga.h>
#include <kb.h>

void kmain(void) {
	//volatile char* video = (volatile char*)0xB8000;
	//video[0] = 'E';
	//video[1] = 0x07;
	//putc('h');
	//putc('\n');
	//putc('i');
	puts("Hello, World!");
	//putc(scancode_to_c(kb_get_scancode()));
	for (;;) {
		putc(loop_until_keypress());
	}
}
