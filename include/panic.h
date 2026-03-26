#pragma once
#ifndef PANIC_H
#define PANIC_H
#include <vga.h>
#include <stddef.h>
#include <idt.h>
extern regs_t *regs;
extern int regs_available;
void oops(const char *msg, ...);
void panic(const char *msg, ...);
#endif
