#pragma once
#include <vga.h>
#include <stddef.h>

void panic(const char *msg) {
	__asm__ volatile ("cli");
	/*puts("\n[ 0.000000] KERNEL PANIC: ");
	puts(msg);
	puts(" system halted!\n");
	puts("[ 0.000000] [No info available]");
	puts("\n[ 0.000000] ---[ end KERNEL PANIC: ");
	puts(msg);
	puts(" system halted! ]---");*/
	/*printk("KERNEL PANIC - system halted!");
	printk(msg);
	printk("---[ end KERNEL PANIC - system halted!");
	printk(msg); // i trust you are mature and won't put weird shit on purpose into the msg buffer
	printk("]---");*/
	set_color(0x04);
	const char *msg2 = msg;
	char buf[1024]; char buf2[1084]; size_t i = 0; size_t j = 0;
	const char *pre = "KERNEL PANIC: ";
	const char *post = " - system halted!";
	const char *_pre = "---[ end KERNEL PANIC: ";
	const char *_post = " - system halted! ]---";
	while (*pre && i < 1023) buf[i++] = *pre++;
	while (*msg && i < 1023) buf[i++] = *msg++;
	while (*post && i < 1023) buf[i++] = *post++;
	buf[i] = '\0';
	printk(buf);
	printk("[No info available]");
	while (*_pre && j < 1023) buf2[j++] = *_pre++;
	while (*msg2 && j < 1023) buf2[j++] = *msg2++;
	while (*_post && j < 1023) buf2[j++] = *_post++;
	buf2[j] = '\0';
	printk(buf2);
	__asm__ volatile ("hlt");
}
