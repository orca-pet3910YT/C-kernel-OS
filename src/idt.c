#include <vga.h>
#include <idt.h>
#include <panic.h>
#include <kb.h>
#include <port.h>
#include <globals.h>
#include <pit.h>

idt_entry_t idt_ents[256];
idtp_t idt_ptr;

static void idt_set_gate(uint8_t n, uint32_t b, uint16_t sel, uint8_t flags) {
	idt_ents[n].base_low = (b & 0xFFFF);
	idt_ents[n].base_high = (b >> 16) & 0xFFFF;
	idt_ents[n].selector = sel;
	idt_ents[n].always_0 = 0;
	idt_ents[n].flags = flags;
}

void init_idt() {
	idt_ptr.limit = sizeof(idt_entry_t)*256-1;
	idt_ptr.base = (uint32_t)&idt_ents;
	for (int i = 0; i < 256; i++) {
		idt_ents[i].base_low = 0;
		idt_ents[i].selector = 0;
		idt_ents[i].always_0 = 0;
		idt_ents[i].flags = 0;
		idt_ents[i].base_high = 0;
	}

	idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
	idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
	idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);
	idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);
	idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E);
	idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E);
	idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E);
	idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8E);
	idt_set_gate(8, (uint32_t)isr8, 0x08, 0x8E);
	idt_set_gate(9, (uint32_t)isr9, 0x08, 0x8E);
	idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
	idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
	idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
	idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
	idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
	idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
	idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
	idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
	idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
	idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
	idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
	idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
	idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
	idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
	idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
	idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
	idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
	idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
	idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
	idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
	idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
	idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);
	idt_set_gate(32, (uint32_t)isr32, 0x08, 0x8E);
	idt_set_gate(33, (uint32_t)isr33, 0x08, 0x8E);
	idt_set_gate(34, (uint32_t)isr34, 0x08, 0x8E);
	idt_set_gate(35, (uint32_t)isr35, 0x08, 0x8E);
	idt_set_gate(36, (uint32_t)isr36, 0x08, 0x8E);
	idt_set_gate(37, (uint32_t)isr37, 0x08, 0x8E);
	idt_set_gate(38, (uint32_t)isr38, 0x08, 0x8E);
	idt_set_gate(39, (uint32_t)isr39, 0x08, 0x8E);
	idt_set_gate(40, (uint32_t)isr40, 0x08, 0x8E);
	idt_set_gate(41, (uint32_t)isr41, 0x08, 0x8E);
	idt_set_gate(42, (uint32_t)isr42, 0x08, 0x8E);
	idt_set_gate(43, (uint32_t)isr43, 0x08, 0x8E);
	idt_set_gate(44, (uint32_t)isr44, 0x08, 0x8E);
	idt_set_gate(45, (uint32_t)isr45, 0x08, 0x8E);
	idt_set_gate(46, (uint32_t)isr46, 0x08, 0x8E);
	idt_set_gate(47, (uint32_t)isr47, 0x08, 0x8E);

	load_idt((uint32_t)&idt_ptr);
}

void isr_handler(regs_t *r) {
	panic("ISR occured! INT %x EIP %x CODE %x", r->int_n, r->eip, r->code);
}

void irq_handler(regs_t *r) {
	//printk("IRQ %x", r->int_n);
	if (r->int_n == 32) {
		pit_tick(r);
	}
	if (r->int_n == 33) {
		char c = scancode_to_c(kb_get_scancode());
		if (c) kbc = c;
	}
	if (r->int_n >= 40) outb(0xA0, 0x20);
	outb(0x20, 0x20);
}
