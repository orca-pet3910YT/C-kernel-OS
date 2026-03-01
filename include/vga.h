#pragma once
#include <stdint.h>
#include <port.h>

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
}


void putc(char c) {
	if (c == '\n') {
		col = 0; row++;
	} else if (c == '\b') {
		if (col == 0 && row > 0) {
			col = 79; row--;
			while (buffer[(row*80+col)-1] == 0x0720 && col != 0) {
				col--;
			}
		} else if (col == 0 && row == 0) {
			return;
		} else {
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
		row = 24; // TODO: scroll
		clear_screen();
	}
	set_cursor_pos(row, col);
}

void puts(const char *s) {
	while (*s) {
		putc(*s++);
	}
}
