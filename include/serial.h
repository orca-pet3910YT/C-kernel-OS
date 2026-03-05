#pragma once
#ifndef SERIAL_H
#define SERIAL_H
void serial_init();
void sputs(const char *s);
void sputc(char c);
char sgetc_raw();
char sgetc();
#endif
