#include <vga.h>

void panic(const char *msg) {
	__asm__ volatile ("cli");
	/*puts("\n[ 0.000000] KERNEL PANIC: ");
	puts(msg);
	puts(" system halted!\n");
	puts("[ 0.000000] [No info available]");
	puts("\n[ 0.000000] ---[ end KERNEL PANIC: ");
	puts(msg);
	puts(" system halted! ]---");*/
	printk("KERNEL PANIC - system halted!");
	printk(msg);
	printk("---[ end KERNEL PANIC - system halted!");
	printk(msg); // i trust you are mature and won't put weird shit on purpose into the msg buffer
	printk("]---");
	__asm__ volatile ("hlt");
}
