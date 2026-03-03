#ifndef KB_H
#define KB_H
#include <stdint.h>
#define SCANCODE_UP 0x48
#define SCANCODE_DOWN 0x50
#define SCANCODE_LEFT 0x4B
#define SCANCODE_RIGHT 0x4D
#define SCANCODE_LSHIFT_PRESS 0x2A
#define SCANCODE_LSHIFT_RELEASE 0xAA
#define SCANCODE_RSHIFT_PRESS 0x36
#define SCANCODE_RSHIFT_RELEASE 0xB6
#define SCANCODE_CTRL_PRESS 0x1D
#define SCANCODE_CTRL_RELEASE 0x9D
#define SCANCODE_ALT_PRESS 0x38
#define SCANCODE_ALT_RELEASE 0xB8
#define SCANCODE_CAPSLOCK 0x3A
#define SCANCODE_EXT 0xE0

uint8_t shift_pressed();
uint8_t ctrl_pressed();
uint8_t alt_pressed();
void kb_init();
void kb_flush_buf();
char scancode_to_c(uint8_t sc);
uint8_t kb_get_scancode();
unsigned char loop_until_keypress();
#endif
