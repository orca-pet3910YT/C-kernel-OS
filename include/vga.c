#include <stdint.h>
#include <port.h>
#include <stdlib.h>
#include <string.h>
#include <globals.h>
#include <serial.h>
#include <vga.h>

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

void clear_screen() {
	for (int i = 0; i < 2000; i++) {
		buffer[i] = 0x0720;
	}
	row = 0; col = 0;
}

void set_ftimestamp(double timestamp, char* buf) {
	int i = 0;
	buf[i++] = '[';
	if (timestamp < 10.0) buf[i++] = ' '; // handle filling the square bracket thingy for our ADHD folks :P
	char num[20] = {0};
	ftoa(timestamp, num);
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
		buffer[24*80+i] = 0x0720;
	}

	return;
}

void putc(char c) {
	if (c == '\n') {
		col = 0; row++;
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
		}
	} else if (c == '\t') {
		for (int i = 0; i < tab_indent; i++) {
			putc(' ');
		}
	} else {
		buffer[row*80+col] = (0x07 << 8) | c; col++;
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

void puts(const char *s) {
	while (*s) {
		putc(*s++);
	}
}

// no more than 1004 chars, thanks
void printk(const char* str) {
	char buf[1024] = {0};
	set_ftimestamp(uptime, buf);
	int i = strlen(buf);
	int j = 0;
	buf[i++] = ' ';
	while (str[j] && i < 1022) {
		buf[i++] = str[j++];
	}
	buf[i] = '\n';
	puts(buf);
	//sputs(buf); // this relies on early serial logging. DO NOT USE printk BEFORE INITIALIZING SERIAL!
}
