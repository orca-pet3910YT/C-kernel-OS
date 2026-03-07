#pragma once
#ifndef VGA_H
#define VGA_H
void set_cursor_pos(int row, int col);
void set_color(unsigned char vga_color);
void clear_screen();
void set_ftimestamp(double timestamp, char *buf);
int putc(int c);
int puts(const char *s);
int print(const char *s, int length); // this can work, but i wouldn't use it if i were you
int printk(const char *str, ...);
int printf(const char *restrict str, ...);
#endif
