#include <vga.h>

void panic(const char *msg) {
	__asm__ volatile ("cli");
	puts("\nKERNEL PANIC: ");
	puts(msg);
	puts(" system halted!\n");
	puts("[No info available]");
	puts("\n---[ end KERNEL PANIC: ");
	puts(msg);
	puts(" system halted! ]---");
	__asm__ volatile ("hlt");
}
