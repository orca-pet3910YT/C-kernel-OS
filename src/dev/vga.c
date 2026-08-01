#include <stdint.h>
#include <arch/i386/port.h>
#include <stdlib.h>
#include <string.h>
#include <sys/globals.h>
#include <drivers/uart/serial.h>
#include <drivers/video/vga.h>
#include <stdarg.h>
#include <limits.h>
#include <drivers/timers/pit.h>
#include <sys/multiboot.h>
#include <drivers/video/font.h>
#include <generated/config.h>

/* VGA table of colors
0 - black - 0x00000000
1 - blue - 0x000000AA
2 - green - 0x0000AA00
3 - bright blue - 0x0000AAAA
4 - red - 0x00AA0000
5 - pink - 0x00AA00AA
6 - shit - 0x00AA5500
7 - gray - 0x00AAAAAA
8 - dark gray - 0x00555555
9 - light blue - 0x005555FF
A - light green - 0x0055FF55
B - brighter blue - 0x0055FFFF
C - light red - 0x00FF5555
D - light pink - 0x00FF55FF
E - yellow - 0x00FFFF55
F - white - 0x00FFFFFF
*/

//unsigned char color = 0x07;
//extern uint32_t bg_color;
//extern uint32_t fg_color;
//uint32_t bg_color = 0x00000000;
//uint32_t fg_color = 0x00DFDFDF;
int row = 0;
int col = 0;
//static uint16_t* buffer = (uint16_t*)0xB8000;
int tab_indent = 4;

/*void set_cursor_pos(int row, int col) {
	unsigned short pos = row*80+col;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (unsigned char)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

void set_color(unsigned char vga_color) {
	color = vga_color;
}

void clear_screen_vga() {
	for (int i = 0; i < 2000; i++) {
		buffer[i] = (color << 8) + 0x20;
	}
	row = 0; col = 0;
}*/

void set_color(uint32_t bg, uint32_t fg) {
#ifdef CONFIG_COLOR_FB_TEXT
#if CONFIG_COLOR_FB_TEXT
	bg_color = bg; fg_color = fg;
#else
	(void)bg; (void)fg;
#endif
#endif /* CONFIG_COLOR_FB_TEXT */
}

void set_ftimestamp(double timestamp, char* buf) {
#ifdef CONFIG_PRINTK_TIME
#if CONFIG_PRINTK_TIME
	int i = 0;
	buf[i++] = '[';
	if (timestamp < 10.0) buf[i++] = ' '; // handle filling the square bracket thingy for our ADHD folks :P
	if (timestamp < 100.0) buf[i++] = ' ';
	if (timestamp < 1000.0) buf[i++] = ' ';
	//if (timestamp > 10000.0) buf[i++] = ' ';
	char num[20] = {0};
	ftoa(timestamp, num, 4);
	for (int j = 0; num[j] != '\0'; j++) {
		buf[i++] = num[j];
	}
	buf[i++] = ']';
	buf[i++] = ' ';
	buf[i] = '\0';
#else
	(void)timestamp; (void)buf;
#endif
#endif /* CONFIG_PRINTK_TIME */
}

/*void scroll_once() {
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
}*/

int putc(int c) {
	if (c == -1) return -1;
	if (!c) return 0;
	//set_color(c); // don't uncomment unless you love unicorn puke
	if (c == '\n') {
		//col = 0; row++;
		if (serial_out) sputc('\n');
	} else if (c == '\b') {
		//if (col == 0 && row == 0) {
			/*col = 79; row--;
			while (buffer[(row*80+col)-1] == 0x0720 && col != 0) {
				col--;
			}*/ // text editor style
			//return 0;
		//} else if (col > 0 && row >= 0 /*2*/) {
			//col--; buffer[row*80+col] = (color << 8) | ' ';
			if (serial_out) sputs("\b \b");
		/*} else if (col == 0 && row > 0) {
			col = 79; row--;
			buffer[row*80+col] = (color << 8) | ' ';
		}*/
	} else if (c == 0x1E) { // up
		return 0;
	} else if (c == 0x1F) { // down
		return 0;
	} else if (c == 0x1A) { // left
		/*if (col < 2) return 0;
		col--; set_cursor_pos(row, col); return 0x1A;*/
		return 0;
	} else if (c == 0x1B) {
		return 0;
	} else if (c == '\t') {
		for (int i = 0; i < tab_indent; i++) {
			putc(' '); //if (serial_out) sputc(' ');
		}
		if (serial_out) sputc('\t');
	} else {
		//buffer[row*80+col] = (color << 8) | c; col++;
		if (serial_out) sputc(c);
	}
	/*if (col >= 80) {
		col = 0; row++;
	}
	if (row >= 25) {
		scroll_once(); //clear_screen();
		row = 24; // 0 // 24
		//col = 0;
	}*/
	//set_cursor_pos(row, col);
	if (font_initialized) put_char(c);
	//if (serial_out) sputc(c);
	return c;
}

// put character extended

/*void putce(char c) {
	if (c == '\n') {
		col = 0; row++;
		sputc(c);
	} else if (c == '\b') {
		if (col == 0 && row == 0) {*/
			/*col = 79; row--;
			while (buffer[(row*80+col)-1] == 0x0720 && col != 0) {
				col--;
			}*/ // text editor style
		/*} else if (col == 0 && row == 0) {
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
}*/

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
			char buf[10]; htoa(value, buf);
			if (maxrem < 10) return -1;
			if (!print(buf, 10)) return -1;
			written += 10;
			// TODO: finish
		} else if (*format == 'd') {
			format++; int value = va_arg(parameters, int);
			char buf[12]; itoa(value, buf);
			if (maxrem < strlen(buf)) return -1;
			if (!print(buf, strlen(buf))) return -1;
			written += strlen(buf);
		} else if (*format == 'u') {
			format++; uint32_t value = va_arg(parameters, int);
			char buf[12]; itoa(value, buf);
			if (maxrem < strlen(buf)) return -1;
			if (!print(buf, strlen(buf))) return -1;
			written += strlen(buf);
		} else if (*format == 'f') {
			format++; double value = va_arg(parameters, double);
			char buf[17] = {0};
			ftoa(value, buf, 16);
			if (maxrem < 17) return -1;
			if (!print(buf, 17)) return -1;
			written += 17;
		} else {
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) return -1;
			if (!print(format, len)) return -1;
			written += len; format += len;
		}
	}
	flush_term();
	return written;
}

int printf(const char *restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);
	int result = cprintf(format, parameters);
	flush_term();
	va_end(parameters);
	return result;
}

// no more than 1004 chars, thanks
int printk(unsigned int pass_loglevel, const char* str, ...) {
	if (pass_loglevel > loglevel) return -1;
	va_list params;
	va_start(params, str);
	char buf[1024] = {0};
	retrieve_uptime();
	set_ftimestamp(uptime, buf);
	int i = strlen(buf);
	int j = 0;
#ifdef CONFIG_PRINTK_TYPE
#if CONFIG_PRINTK_TYPE
	//i++;
	*(buf+(i++)) = '[';
	i++;
	switch (pass_loglevel) {
		case 0:
			*(buf+i-1) = 'e';
			break;
		case 1:
			*(buf+i-1) = 'c';
			break;
		case 2:
			*(buf+i-1) = 'r';
			break;
		case 3:
			*(buf+i-1) = 'w';
			break;
		case 4:
			*(buf+i-1) = 'i';
			break;
		case 5:
			*(buf+i-1) = 't';
			break;
		case 6:
			*(buf+i-1) = 'd';
			break;
		case 7:
			*(buf+i-1) = 'u';
			break;
		default:
			*(buf+i-1) = '?';
	}
	*(buf+(i++)) = ']';
	*(buf+(i++)) = ' ';
#endif
#endif
	while (str[j] && i < 1022) {
		buf[i++] = str[j++];
	}
	buf[i] = '\n';
	int r = cprintf(buf, params);
	va_end(params);
	return r;
	//sputs(buf); // this relies on early serial logging. DO NOT USE printk BEFORE INITIALIZING SERIAL!
}

int cprintk(unsigned int pass_loglevel, const char *str, va_list params) {
	if (pass_loglevel > loglevel) return -1;
	char buf[1024] = {0};
	retrieve_uptime();
	set_ftimestamp(uptime, buf);
	int i = strlen(buf);
	int j = 0;
	#ifdef CONFIG_PRINTK_TYPE
	#if CONFIG_PRINTK_TYPE
	//i++;
	*(buf+(i++)) = '[';
	i++;
	switch (pass_loglevel) {
		case 0:
			*(buf+i-1) = 'e';
			break;
		case 1:
			*(buf+i-1) = 'c';
			break;
		case 2:
			*(buf+i-1) = 'r';
			break;
		case 3:
			*(buf+i-1) = 'w';
			break;
		case 4:
			*(buf+i-1) = 'i';
			break;
		case 5:
			*(buf+i-1) = 't';
			break;
		case 6:
			*(buf+i-1) = 'd';
			break;
		case 7:
			*(buf+i-1) = 'u';
			break;
		default:
			*(buf+i-1) = '?';
	}
	*(buf+(i++)) = ']';
	*(buf+(i++)) = ' ';
	#endif
	#endif
	while (str[j] && i < 1022) {
		buf[i++] = str[j++];
	}
	buf[i] = '\n';
	int r = cprintf(buf, params);
	return r;
}

void draw_rect(int x, int y, int w, int h, uint32_t color) {
	uint32_t *fb = framebuffer_info->fb;
	uint32_t pitch = framebuffer_info->pitch;
	for (int fy = y; fy < y+h; fy++) {
		uint32_t *row = fb+(pitch/4*fy);
		for (int fx = x; fx < x+w; fx++) {
			row[fx] = color;
		}
	}
}

void draw_rect_outline(int x, int y, int w, int h, uint32_t color, int thickness) {
	draw_rect(x, y, w, thickness, color);
	draw_rect(x, y+h-thickness, w, thickness, color);
	draw_rect(x, y, thickness, h, color);
	draw_rect(x+w-thickness, y, thickness, h, color);
}
