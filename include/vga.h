#pragma once
#ifndef VGA_H
#define VGA_H
void set_cursor_pos(int row, int col);
void set_color(unsigned char vga_color);
void clear_screen();
void set_ftimestamp(double timestamp, char *buf);
void putc(char c);
void puts(const char *s);
void printk(const char *str);
#endif
