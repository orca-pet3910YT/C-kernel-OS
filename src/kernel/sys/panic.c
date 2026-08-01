#include <drivers/video/vga.h>
#include <sys/panic.h>
#include <stddef.h>
#include <stdarg.h>
#include <arch/i386/idt.h> // regs_t
#include <sys/globals.h> // loglevel
#include <drivers/video/font.h> // term flush
#include <generated/config.h>
#include <string.h>

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
regs_t *regs;
int regs_available = 0;

void oops(const char *msg, ...) {
	char log_old = loglevel;
	loglevel = 7;
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
	set_color(0x000000AA, 0x00FFFFFF);
	msg2 = msg;
        // first header
        while (*__oops_pre && __oops_i < 1023) __oops_buf[__oops_i++] = *__oops_pre++;
        while (*msg && __oops_i < 1023) __oops_buf[__oops_i++] = *msg++;
        while (*__oops_post && __oops_i < 1023) __oops_buf[__oops_i++] = *__oops_post++;
        __oops_buf[__oops_i] = '\0';
        cprintk(0, __oops_buf, params);
        // TODO: stack trace of function addresses + distinguish from regular values
        printk(0, "Oopses triggered: %d", oopses);
	if (regs_available) {
		printk(0, "EAX: %x EBX: %x ECX: %x EDX: %x", regs->eax, regs->ebx, regs->ecx, regs->edx);
		printk(0, "At %x:%x accessing %x:%x, EBP: %x, ESP: %x", regs->cs, regs->eip, regs->ds, regs->edi, regs->ebp, regs->esp);
		printk(0, "EFLAGS: %x", regs->eflags);
		printk(0, "Code: %x %x %x %x", *(int*)regs->eip, *((int*)(regs->eip)+4), *((int*)(regs->eip)+8), *((int*)(regs->eip)+12));
	}
        // second header (or the end header)
        while (*__oops__pre && __oops_j < 1023) __oops_buf2[__oops_j++] = *__oops__pre++;
        while (*msg2 && __oops_j < 1023) __oops_buf2[__oops_j++] = *msg2++;
        while (*__oops__post && __oops_j < 1023) __oops_buf2[__oops_j++] = *__oops__post++;
        __oops_buf2[__oops_j] = '\0'; // NULL terminate
        cprintk(0, __oops_buf2, params);
        va_end(params);
	if (oopses >= 3) {
		panic("Oopsed three times");
		__asm__ volatile ("hlt");
	}
	set_color(0x00000000, 0x00AAAAAA); oopsing = 0;
	__asm__ volatile ("sti");
	loglevel = log_old;
	flush_term();
}

// panic rewrites: 5
// please increment the above number
// in an event of a big update to panic

void panic(const char *msg, ...) {
	loglevel = 0; // don't even bother saving the old value. why? panic never returns?
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
		printk(0, "DOUBLE PANIC - SYSTEM HALTED");
		__asm__ volatile ("cli; hlt");
	}
	panicking = 1;
	va_list params;
	va_start(params, msg);
#if CONFIG_PRETTY_PANIC
	set_color(0x001FFF1F, 0x00000000); // 0x000000AA, 0x00FFFFFF
	//clear_screen();
	tx = 5; ty = 5;
	printf("This machine ran into an irrecoverable error. See logs below for more information.\n");
	tx = 0; ty = 10;
#else
	set_color(0x000000AA, 0x00FFFFFF);
#endif
	__asm__ volatile ("cli");
	msg3 = msg;
	// first header
	while (*__panic_pre && __panic_i < 1023) __panic_buf[__panic_i++] = *__panic_pre++;
	while (*msg && __panic_i < 1023) __panic_buf[__panic_i++] = *msg++;
	while (*__panic_post && __panic_i < 1023) __panic_buf[__panic_i++] = *__panic_post++;
	__panic_buf[__panic_i] = '\0';
	cprintk(0, __panic_buf, params);
	// TODO: stack trace of function addresses + distinguish from regular values
	printk(0, "---BEGIN Panic info---");
	if (oopses > 0) printk(0, "Oopses triggered: %d", oopses);
	if (regs_available) {
		printk(0, "EAX: %x EBX: %x ECX: %x EDX: %x", regs->eax, regs->ebx, regs->ecx, regs->edx);
		printk(0, "At %x:%x accessing %x:%x, EBP: %x, ESP: %x", regs->cs, regs->eip, regs->ds, regs->edi, regs->ebp, regs->esp);
		printk(0, "EFLAGS: %x", regs->eflags);
		if (regs->eip > 0x00100000 && regs->eip < 0x07ffffff) printk(0, "Code: %x %x %x %x", reverse(*(int*)regs->eip), reverse(*((int*)(regs->eip)+1)), reverse(*((int*)(regs->eip)+2)), reverse(*((int*)(regs->eip)+3)));
	}
	printk(0, "--- END Panic info ---");
	// second header (or the end header)
	while (*__panic__pre && __panic_j < 1023) __panic_buf2[__panic_j++] = *__panic__pre++;
	while (*msg3 && __panic_j < 1023) __panic_buf2[__panic_j++] = *msg3++;
	while (*__panic__post && __panic_j < 1023) __panic_buf2[__panic_j++] = *__panic__post++;
	__panic_buf2[__panic_j] = '\0'; // NULL terminate
	cprintk(0, __panic_buf2, params);
	__asm__ volatile ("hlt");
}
