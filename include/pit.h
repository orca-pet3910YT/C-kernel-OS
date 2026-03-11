#ifndef PIT_H
#define PIT_H
#define PIT_BASE 1193182
#include <stdint.h>
#include <idt.h> // registers_t
void init_pit();
void pit_shutdown();
void pit_set_div(uint16_t div);
void pit_tick(regs_t *r);
#endif
