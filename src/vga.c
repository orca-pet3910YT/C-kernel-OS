#include <stdint.h>
#include <port.h>
#include <stdlib.h>
#include <string.h>
#include <globals.h>
#include <serial.h>
#include <vga.h>
#include <stdarg.h>
#include <limits.h>

/* VGA table of colors
0 - black
1 - blue
2 - green
3 - bright blue
4 - red
5 - pink
6 - shit
7 - gray
8 - dark gray
9 - light blue
A - light green
B - brighter blue
C - light red
D - light pink
E - yellow
F - white
*/

unsigned char color = 0x07;
int row = 0;
int col = 0;
static uint16_t* buffer = (uint16_t*)0xB8000;
int tab_indent = 4;

void set_cursor_pos(int row, int col) {
	unsigned short pos = row*80+col;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (unsigned char)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

void set_color(unsigned char vga_color) {
	color = vga_color;
}

void clear_screen() {
	for (int i = 0; i < 2000; i++) {
		buffer[i] = (color << 8) + 0x20;
	}
	row = 0; col = 0;
}

void set_ftimestamp(double timestamp, char* buf) {
	int i = 0;
	buf[i++] = '[';
	if (timestamp < 10.0) buf[i++] = ' '; // handle filling the square bracket thingy for our ADHD folks :P
	if (timestamp < 100.0) buf[i++] = ' ';
	if (timestamp < 1000.0) buf[i++] = ' ';
	char num[20] = {0};
	ftoa(timestamp, num, 4);
	for (int j = 0; num[j] != '\0'; j++) {
		buf[i++] = num[j];
	}
	buf[i++] = ']';
	buf[i] = '\0';
}

void scroll_once() {
	//int index = 0;
	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 80; j++) {
			buffer[i*80+j] = buffer[(i+1)*80+j];
		}
	}
	for (int i = 0; i < 80; i++) {
		buffer[24*80+i] = (color << 8) + 0x20;
	}

	return;
}

int putc(int c) {
	if (c == -1) return -1;
	if (c == '\n') {
		col = 0; row++;
		if (serial_out) sputc('\n');
	} else if (c == '\b') {
		if (col == 0 && row == 0) {
			/*col = 79; row--;
			while (buffer[(row*80+col)-1] == 0x0720 && col != 0) {
				col--;
			}*/ // text editor style
		} else if (col == 0 && row == 0) {
			return 0;
		} else if (col > 2) {
			col--; buffer[row*80+col] = (color << 8) | ' ';
			if (serial_out) sputc(c);
		}
	} else if (c == '\t') {
		for (int i = 0; i < tab_indent; i++) {
			putc(' '); if (serial_out) sputc(' ');
		}
	} else {
		buffer[row*80+col] = (color << 8) | c; col++;
		if (serial_out) sputc(c);
	}
	if (col >= 80) {
		col = 0; row++;
	}
	if (row >= 25) {
		scroll_once(); //clear_screen();
		row = 24; // 0 // 24
		//col = 0;
	}
	set_cursor_pos(row, col);
	//if (serial_out) sputc(c);
	return c;
}

// put character extended

void putce(char c) {
	if (c == '\n') {
		col = 0; row++;
		sputc(c);
	} else if (c == '\b') {
		if (col == 0 && row == 0) {
			/*col = 79; row--;
			while (buffer[(row*80+col)-1] == 0x0720 && col != 0) {
				col--;
			}*/ // text editor style
		} else if (col == 0 && row == 0) {
			return;
		} else if (col > 2) {
			col--; buffer[row*80+col] = (0x07 << 8) | ' ';
			sputc(c);
		}
	} else if (c == '\t') {
		for (int i = 0; i < tab_indent; i++) {
			putc(' ');
		}
	} else {
		buffer[row*80+col] = (0x07 << 8) | c; col++;
		sputc(c);
	}
	if (col >= 80) {
		col = 0; row++;
	}
	if (row >= 25) {
		scroll_once(); //clear_screen();
		row = 24; // 0 // 24
		//col = 0;
	}
	set_cursor_pos(row, col);
}

int puts(const char *s) {
	while (*s) {
		if (putc(*s++) == -1) return 0;
	}
	return 1;
}

int print(const char *s, int length) {
	for (int i = 0; i < length; i++) {
		if (putc(s[i]) == -1) return 0;
	}
	return 1;
}

int cprintf(const char *restrict format, va_list parameters) {
	int written = 0;
	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;
		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%') format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%') amount++;
			if (maxrem < amount) return -1;
			if (!print(format, amount)) return -1;
			format += amount; written += amount;
			continue;
		}
		const char *format_begun_at = format++;
		if (*format == 'c') {
			format++;
			char c = (char)va_arg(parameters, int);
			if (!maxrem) return -1;
			if (!print(&c, sizeof(c))) return -1;
			written++;
		} else if (*format == 's') {
			format++; const char *str = va_arg(parameters, const char*);
			if (!str) str = "(Null)";
			size_t len = strlen(str);
			if (maxrem < len) return -1;
			if (!print(str, len)) return -1;
			written += len;
		} else if (*format == 'x') {
			format++; unsigned int value = va_arg(parameters, unsigned int);
			char buf[8]; htoa(value, buf);
			if (maxrem < 8) return -1;
			if (!print(buf, 8)) return -1;
			written += 8;
			// TODO: finish
		} else {
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) return -1;
			if (!print(format, len)) return -1;
			written += len; format += len;
		}
	}
	return written;
}

int printf(const char *restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);
	int result = cprintf(format, parameters);
	va_end(parameters);
	return result;
}

// no more than 1004 chars, thanks
int printk(const char* str, ...) {
	va_list params;
	va_start(params, str);
        char buf[1024] = {0};
        set_ftimestamp(uptime, buf);
        int i = strlen(buf);
        int j = 0;
        buf[i++] = ' ';
        while (str[j] && i < 1022) {
                buf[i++] = str[j++];
        }
        buf[i] = '\n';
	int r = cprintf(buf, params);
	va_end(params);
        return r;
        //sputs(buf); // this relies on early serial logging. DO NOT USE printk BEFORE INITIALIZING SERIAL!
}
