#include <arch/i386/tss.h>
#include <arch/i386/gdt.h>
#include <string.h>
tss_entry_t tss_entry;
void flush_tss(void); // comes from tss.asm
void write_tss(gdt_entry_bits *g) {
	uint32_t base = (uint32_t)&tss_entry;
	uint32_t limit = sizeof(tss_entry);
	g->limit_low = limit;
	g->base_low = base;
	g->accessed = 1;
	g->read_write = 0;
	g->conforming_expand_down = 0;
	g->code = 1;
	g->code_data_segment = 0;
	g->dpl = 0;
	g->present = 1;
	g->limit_high = (limit & (0x0F << 16)) >> 16;
	g->available = 0;
	g->long_mode = 0;
	g->big = 0;
	g->gran = 0;
	g->base_high = (base & (0xFF << 24)) >> 24;
	memset(&tss_entry, 0, sizeof(tss_entry));
	unsigned short ss;
	__asm__ volatile ("mov %%ss, %0" : "=r" (ss)); // retrieve ss
	extern unsigned int kernel_stack; // defined in boot.s
	tss_entry.ss0 = ss;
	tss_entry.esp0 = kernel_stack;
	// TODO finish
}

void set_kernel_stack(uint32_t stack_ptr) {
	tss_entry.esp0 = stack_ptr;
}
