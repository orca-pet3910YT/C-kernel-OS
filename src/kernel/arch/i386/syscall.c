#include <arch/i386/idt.h> // for regs_t
#include <sys/globals.h>
#include <drivers/video/vga.h>
#include <error.h>

typedef struct {
	uint32_t eax; uint32_t ebx;
	uint32_t ecx; uint32_t edx;
	uint32_t edi; uint32_t esi;
	uint32_t ebp;
} sysreg_t;


/*sysreg_t sys_time(sysreg_t *r) {
	r->eax = 0;
	r->ebx
*/ // please figure this out eventually

void sys_ph(sysreg_t *r) { // placeholder
	r->eax = 0;
}

/*
 * now before you start shitting and pissing,
 * this is temporary. i'll make this a proper write() call later,
 * when i get less lazy and make the VFS better.
 */
void sys_write(sysreg_t *r) {
	if (r->ebx == 1) print((char*)r->ecx, r->edx);
	r->eax = 0;
}

static void (*syscall_handlers[])(sysreg_t*) = {
	sys_ph,
	sys_ph,
	sys_ph,
	sys_ph,
	sys_write,
	sys_ph,
	sys_ph,
	sys_ph,
	sys_ph,
	sys_ph,
	sys_ph,
	sys_ph,
	sys_ph,
	sys_ph, //sys_time,
};

static const int no_handlers = sizeof(syscall_handlers)/sizeof(*syscall_handlers);

void handle_syscall(sysreg_t *r) {
	printk(6, "received syscall %d", r->eax);
	if (r->eax > no_handlers) { // we can't exceed the number because of our array being finite
		r->eax = EBADARG;
		return;
	}
	syscall_handlers[r->eax](r);
}

