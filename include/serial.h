#pragma once
#ifndef SERIAL_H
#define SERIAL_H
void serial_init();
void serial_shutdown();
void sputs(const char *s);
void sputc(char c);
int sgetc_raw();
char sgetc();
#endif
