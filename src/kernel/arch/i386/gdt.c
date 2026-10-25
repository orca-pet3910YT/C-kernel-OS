#include <stdint.h>
#include <arch/i386/gdt.h>
#include <arch/i386/tss.h>

gdt_ent_t gdt[6];
gdt_ptr_t gdt_p;
extern tss_entry_t tss_entry;

extern void gdt_flush(uint32_t);

static void gdt_set_gate(int n, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
	gdt[n].base_low = (base & 0xFFFF);
	gdt[n].base_middle = (base >> 16) & 0xFF;
	gdt[n].base_high = (base >> 24) & 0xFF;
	gdt[n].limit_low = (limit & 0xFFFF);
	gdt[n].granularity = (limit >> 16) & 0x0F;
	gdt[n].granularity |= gran & 0xF0;
	gdt[n].access = access;
}

void gdt_init() {
	gdt_p.limit = sizeof(gdt_ent_t)*6-1;
	gdt_p.base = (uint32_t)&gdt;
	gdt_set_gate(0, 0, 0, 0, 0);
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
	//write_tss(&gdt[5]);
	tss_init();
	gdt_set_gate(5, (uint32_t)&tss_entry, sizeof(tss_entry)-1, 0xE9, 0);
	gdt_flush((uint32_t)&gdt_p);
	flush_tss();
}
