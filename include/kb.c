/*
 * heavily inspired by aOS by Aarav Mehta and aOS contributors' /src/dev/keyboard.c
 * thanks a lot for the inspiration to make a kernel!
 */

#include "port.h"
#include "kb.h"

static uint8_t shift = 0;
static uint8_t ctrl = 0;
static uint8_t alt = 0;
static uint8_t capslock = 0;
static uint8_t ext = 0;

static const char scancode_map_lower[] = {
	0,    0,   '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',  '-',  '=',  0,    0,
	'q',  'w', 'e',  'r',  't',  'y',  'u',  'i',  'o',  'p',  '[',  ']',  '\n', 0,    'a',  's',
	'd',  'f', 'g',  'h',  'j',  'k',  'l',  ';',  '\'', '`',  0,    '\\', 'z',  'x',  'c',  'v',
	'b',  'n', 'm',  ',',  '.',  '/',  0,    '*',  0,    ' ',  0,    0,    0,    0,    0,    0,
	0,    0,   0,    0,    0,    0,    0,    '7',  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
	'2',  '3', '0',  '.',  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0
};
static const char scancode_map_upper[] = {
	0,    0,   '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',  '_',  '+',  0,    0,
	'Q',  'W', 'E',  'R',  'T',  'Y',  'U',  'I',  'O',  'P',  '{',  '}',  '\n', 0,    'A',  'S',
	'D',  'F', 'G',  'H',  'J',  'K',  'L',  ':',  '"',  '~',  0,    '|',  'Z',  'X',  'C',  'V',
	'B',  'N', 'M',  '<',  '>',  '?',  0,    '*',  0,    ' ',  0,    0,    0,    0,    0,    0,
	0,    0,   0,    0,    0,    0,    0,    '7',  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
	'2',  '3', '0',  '.',  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0
};

uint8_t shift_pressed() { return shift; }
uint8_t ctrl_pressed() { return ctrl; }
uint8_t alt_pressed() { return alt; }

void kb_init() {
	outb(0x64, 0xAE);
	shift = 0; ctrl = 0; alt = 0; capslock = 0;
	kb_flush_buf();
}

void kb_flush_buf() {
	for (int i = 0; i < 16; i++) {
		uint8_t status = inb(0x64);
		if (status & 0x01) {
			inb(0x60);
		} else { break; }
	}
}

char scancode_to_c(uint8_t sc) {
	if (sc == SCANCODE_EXT) {
		ext = 1;
		return 0;
	}
	if (ext) {
		ext = 0;
		if (sc == SCANCODE_UP) return 0x1E; // up
		if (sc == SCANCODE_DOWN) return 0x1F;
		if (sc == SCANCODE_LEFT) return 0x1A;
		if (sc == SCANCODE_RIGHT) return 0x1B;
		if (sc & 0x80) return 0;
		return 0;
	}
	if (sc == SCANCODE_LSHIFT_PRESS || sc == SCANCODE_RSHIFT_PRESS) { shift = 1; return 0; }
	if (sc == SCANCODE_LSHIFT_RELEASE || sc == SCANCODE_RSHIFT_RELEASE) { shift = 0; return 0; }
	if (sc == SCANCODE_CTRL_PRESS) { ctrl = 1; return 0; }
	if (sc == SCANCODE_CTRL_RELEASE) { ctrl = 0; return 0; }
	if (sc == SCANCODE_ALT_PRESS) { alt = 1; return 0; }
	if (sc == SCANCODE_ALT_RELEASE) { alt = 0; return 0; }
	if (sc == SCANCODE_CAPSLOCK)  { capslock = !capslock; return 0; }
	if (sc & 0x80) return 0;
	if (sc == 0x1C) return '\n';
	if (sc == 0x0E) return '\b';
	if (sc == 0x0F) return '\t';
	if (sc == 0x39) return ' '; // space
	if (sc == 0x01) return 0x1B; // escape
	if (sc >= sizeof(scancode_map_lower)) return 0;
	char c;
	if (shift) { c = scancode_map_upper[sc]; }
	else       { c = scancode_map_lower[sc]; }
	if      (capslock && c >= 'a' && c <= 'z') { c = c - 'a' + 'A'; }
	else if (capslock && c >= 'A' && c <= 'Z' && !shift) {}
	else if (capslock && c >= 'A' && c <= 'Z' && shift ) { c = c - 'A' + 'a'; }
	return c; // finally holy shit
}

uint8_t kb_get_scancode() {
	uint8_t status = inb(0x64);
	for (int i = 0; i < 16; i++) {
		if (status & 0x01) {
		uint8_t data = inb(0x60); // keyboard port
			if (status & 0x20) { // mouse data - but we're accessing KB data right? right!
				return 0;
			}
		return data; // data
		}
	}
	return 0; // no data
}

unsigned char loop_until_keypress() {
	for (;;) {
		char c = scancode_to_c(kb_get_scancode());
		if (c) return c;
	}
}
