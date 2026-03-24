#include <vga.h>
#include <panic.h>
#include <stddef.h>
#include <stdarg.h>

// `__panic_` variables. they are defined here to avoid putting them in the stack

char __panic_buf[1024];
char __panic_buf2[1084];
char __oops_buf[1024];
char __oops_buf2[1084];
size_t __oops_i = 0; size_t __oops_j = 0;
size_t __panic_i = 0; size_t __panic_j = 0;
const char *__panic_pre = "*** PANIC *** <";
const char *__panic_post = "> - system halted!";
const char *__panic__pre = "---[ end *** PANIC *** <";
const char *__panic__post = "> - system halted! ]---";
const char *__oops_pre = "Oops! <";
const char *__oops_post = "> - beware!";
const char *__oops__pre = "---[ end Oops! <";
const char *__oops__post = "> - beware! ]---";
const char *msg2; const char *msg3;
int oopses = 0; // reaching three oopses will cause a panic
int oopsing = 0;
int panicking = 0;

void oops(const char *msg, ...) {
	__asm__ volatile ("cli");
	__oops_pre = "Oops! <";
	__oops_post = "> - beware!";
	__oops__pre = "---[ end Oops! <";
	__oops__post = "> - beware! ]---";
	msg2 = ""; __oops_i = 0; __oops_j = 0;
	__oops_buf[0] = 0; __oops_buf2[0] = 0;
	oopses++;
	if (oopsing) panic("Oops while oops");
	oopsing = 1;
	va_list params;
	va_start(params, msg);
	set_color(0x1F);
	msg2 = msg;
        // first header
        while (*__oops_pre && __oops_i < 1023) __oops_buf[__oops_i++] = *__oops_pre++;
        while (*msg && __oops_i < 1023) __oops_buf[__oops_i++] = *msg++;
        while (*__oops_post && __oops_i < 1023) __oops_buf[__oops_i++] = *__oops_post++;
        __oops_buf[__oops_i] = '\0';
        cprintk(__oops_buf, params);
        // TODO: stack trace of function addresses + distinguish from regular values
        printk("Oopses triggered: %d", oopses);
        // second header (or the end header)
        while (*__oops__pre && __oops_j < 1023) __oops_buf2[__oops_j++] = *__oops__pre++;
        while (*msg2 && __oops_j < 1023) __oops_buf2[__oops_j++] = *msg2++;
        while (*__oops__post && __oops_j < 1023) __oops_buf2[__oops_j++] = *__oops__post++;
        __oops_buf2[__oops_j] = '\0'; // NULL terminate
        cprintk(__oops_buf2, params);
        va_end(params);
	if (oopses >= 3) {
		panic("Oopsed three times");
		__asm__ volatile ("hlt");
	}
	set_color(0x07); oopsing = 0;
	__asm__ volatile ("sti");
}

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
	if (panicking) {
		printk("**** DOUBLE PANIC - SYSTEM HALTED ****");
		__asm__ volatile ("hlt");
	}
	panicking = 1;
	va_list params;
	va_start(params, msg);
	set_color(0x1F);
	msg3 = msg;
	// first header
	while (*__panic_pre && __panic_i < 1023) __panic_buf[__panic_i++] = *__panic_pre++;
	while (*msg && __panic_i < 1023) __panic_buf[__panic_i++] = *msg++;
	while (*__panic_post && __panic_i < 1023) __panic_buf[__panic_i++] = *__panic_post++;
	__panic_buf[__panic_i] = '\0';
	cprintk(__panic_buf, params);
	// TODO: stack trace of function addresses + distinguish from regular values
	printk("---BEGIN Panic info---");
	if (oopses > 0) printk("Oopses triggered: %d", oopses);
	printk("--- END Panic info ---");
	// second header (or the end header)
	while (*__panic__pre && __panic_j < 1023) __panic_buf2[__panic_j++] = *__panic__pre++;
	while (*msg3 && __panic_j < 1023) __panic_buf2[__panic_j++] = *msg3++;
	while (*__panic__post && __panic_j < 1023) __panic_buf2[__panic_j++] = *__panic__post++;
	__panic_buf2[__panic_j] = '\0'; // NULL terminate
	cprintk(__panic_buf2, params);
	va_end(params);
	__asm__ volatile ("hlt");
}
