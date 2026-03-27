#include <pit.h>
#include <vga.h>
#include <port.h>
#include <stdlib.h>
#include <globals.h>

void init_pit() {
	pit_set_div(1429);
}

void pit_shutdown() {
	outb(0x43, 0x30);
	outb(0x40, 0x00);
	outb(0x40, 0x00);
}

void pit_set_div(uint16_t div) {
	uint16_t hertz = PIT_BASE/div;
	outb(0x43, 0x36);
	wait_port();
	outb(0x40, (uint8_t)(hertz & 0xFF));
	wait_port();
	outb(0x40, (uint8_t)((hertz >> 8) & 0xFF));
	wait_port();
}

void pit_tick(regs_t *r) {
	(void)r;
	uptime += 0.0007;
	uptime_ticks++;
}
