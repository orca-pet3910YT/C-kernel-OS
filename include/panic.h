#pragma once
#ifndef PANIC_H
#define PANIC_H
#include <vga.h>
#include <stddef.h>
void panic(const char *msg, ...);
#endif
