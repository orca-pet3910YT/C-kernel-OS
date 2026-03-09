#ifndef STDLIB
#define STDLIB
#include <stdint.h>
int itoa(uint32_t n, char *str);
int ftoa(double n, char *str);
int htoa(uint32_t n, char *str);
#define NULL 0
#define true 1
#define false 0
typedef _Bool bool;
#endif
