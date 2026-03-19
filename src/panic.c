#include <vga.h>
#include <panic.h>
#include <stddef.h>
#include <stdarg.h>

char __panic_buf[1024];
char __panic_buf2[1084];
size_t __panic_i = 0; size_t __panic_j = 0;
const char *__panic_pre = "*** PANIC *** <";
const char *__panic_post = "> - system halted!";
const char *__panic__pre = "---[ end *** PANIC *** <";
const char *__panic__post = "> - system halted! ]---";
const char *msg2;

// panic rewrites: 4
// please increment the above number
// in an event of a big update to panic

void panic(const char *msg, ...) {
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
	va_list params;
	va_start(params, msg);
	set_color(0x1F);
	msg2 = msg;
	while (*__panic_pre && __panic_i < 1023) __panic_buf[__panic_i++] = *__panic_pre++;
	while (*msg && __panic_i < 1023) __panic_buf[__panic_i++] = *msg++;
	while (*__panic_post && __panic_i < 1023) __panic_buf[__panic_i++] = *__panic_post++;
	__panic_buf[__panic_i] = '\0';
	cprintk(__panic_buf, params);
	printk("[No info available]");
	while (*__panic__pre && __panic_j < 1023) __panic_buf2[__panic_j++] = *__panic__pre++;
	while (*msg2 && __panic_j < 1023) __panic_buf2[__panic_j++] = *msg2++;
	while (*__panic__post && __panic_j < 1023) __panic_buf2[__panic_j++] = *__panic__post++;
	__panic_buf2[__panic_j] = '\0';
	cprintk(__panic_buf2, params);
	va_end(params);
	__asm__ volatile ("hlt");
}
