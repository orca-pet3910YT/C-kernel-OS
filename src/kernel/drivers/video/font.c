#include <drivers/video/font.h>
#include <stdint.h>
#include <sys/multiboot.h>
#include <string.h>
#include <sys/globals.h>

#define T_CHAR_WIDTH 8
#define T_CHAR_HEIGHT 16
#define T_TABWIDTH 8
typedef struct {
	uint32_t fg;
	uint32_t bg;
	unsigned char dirty;
	unsigned char c;
} term_cell_t;

static term_cell_t terminal[350*100] = {0};
int t_width = 0;
int t_height = 0;
int tx = 0; int ty = 0;
uint32_t pitch = 0;
uint32_t pitchp = 0;
uint32_t fb_bpp = 0;
uint16_t logo_start_x = 0;
uint16_t logo_start_y = 16;

extern uint8_t _binary_bin_default_8x16_psf_start[];
static psf1_head_t *font_head;

void clear_screen(); // required in order for font_init to call the function before its definition to avoid excessive code movement

void font_init() {
	font_head = (psf1_head_t*)_binary_bin_default_8x16_psf_start;
	if (font_head->magic != PSF1_MAGIC) {
		return;
	}
	t_width = framebuffer_info->w/T_CHAR_WIDTH;
	t_height = framebuffer_info->h/T_CHAR_HEIGHT;
	pitch = framebuffer_info->pitch;
	fb_bpp = framebuffer_info->bpp;
	pitchp = (pitch*8)/fb_bpp;
#if CONFIG_GLOGO
	logo_start_x = t_width-32;
#endif
	clear_screen();
}

void _putc(unsigned char c, int x, int y, uint32_t fg, uint32_t bg) {
	if (c) {
		uint8_t *glyph = (uint8_t*)_binary_bin_default_8x16_psf_start+4+(c*font_head->char_size);
		for (int r = 0; r < font_head->char_size; r++) {
			uint32_t *row = framebuffer_info->fb+(y+r)*pitchp;
			uint8_t bits = glyph[r];
			row[x] = (bits & 0x80) ? fg : bg;
			row[x+1] = (bits & 0x40) ? fg : bg;
			row[x+2] = (bits & 0x20) ? fg : bg;
			row[x+3] = (bits & 0x10) ? fg : bg;
			row[x+4] = (bits & 0x08) ? fg : bg;
			row[x+5] = (bits & 0x04) ? fg : bg;
			row[x+6] = (bits & 0x02) ? fg : bg;
			row[x+7] = (bits & 0x01) ? fg : bg;
		}
	}
}

void redraw_term() {
	int x, y;
	for (y = 0; y < t_height; y++) {
		for (x = 0; x < t_width; x++) {
			term_cell_t c = terminal[y*t_width+x];
			_putc(c.c, x*T_CHAR_WIDTH, y*T_CHAR_HEIGHT, c.fg, c.bg);
		}
	}
#if CONFIG_GLOGO
	draw_logo(framebuffer_info);
#endif
}

void clear_screen() {
	memset(framebuffer_info->fb, 0x00, framebuffer_info->h*framebuffer_info->pitch);
	for (int x = 0; x < t_width; x++) {
		for (int y = 0; y < t_height; y++) {
			int offset = y*t_width+x;
			terminal[offset].c = 0x20;
			terminal[offset].fg = fg_color;
			terminal[offset].bg = bg_color;
		}
	}
	redraw_term(); tx = 0; ty = 0;
}

__attribute__((target("sse2"))) void flush_term() {
	for (int y = 0; y < t_height; y++) {
		term_cell_t *c = &terminal[y*t_width];
		for (int x = 0; x < t_width; x++) {
			if (!c->dirty) {
				c++; continue;
			}
#if CONFIG_GLOGO
			if (x < logo_start_x || (x > logo_start_x && y > logo_start_y)) {
				_putc(c->c, x*T_CHAR_WIDTH, y*T_CHAR_HEIGHT, c->fg, c->bg);
				c->dirty = 0;
			}
#else
			_putc(c->c, x*T_CHAR_WIDTH, y*T_CHAR_HEIGHT, c->fg, c->bg);
			c->dirty = 0;
#endif
			c++;
		}
	}
}

__attribute__((target("sse2"))) void scroll_term() {
	drv_dbg[0] = uptime_ticks;
	term_cell_t *term = terminal;
	memmove(term, term+t_width, sizeof(term_cell_t)*t_width*(t_height-1));
	drv_dbg[1] = uptime_ticks;
	term_cell_t *clear_cell = &terminal[(t_height-1)*t_width];
	for (int x = 0; x < t_width; x++) {
		clear_cell->c = ' '; clear_cell->fg = fg_color; (clear_cell++)->bg = bg_color;
	}
	drv_dbg[2] = uptime_ticks;
	for (int i = 0; i < t_width*t_height; i++) terminal[i].dirty = 1;
	ty = t_height - 1; tx = 0;
	drv_dbg[3] = uptime_ticks;
}

void put_char(char c) {
	if (c == '\n') {
		tx = 0; ty++;
		if (ty >= t_height) scroll_term();
		return;
	}
	if (c == '\b' && tx > 0) {
		tx--;
		int offset = ty*t_width+tx;
		terminal[offset].fg = fg_color;
		terminal[offset].bg = bg_color;
		terminal[offset].dirty = terminal[ty*t_width+tx].c == ' ' ? 0 : 1;
		terminal[offset].c = ' ';
		return;
	}
	if (c == '\t') {
		tx = (tx + T_TABWIDTH) & ~(T_TABWIDTH - 1);
		return;
	}
	if (ty >= t_height) scroll_term();
	term_cell_t *cell = &terminal[ty*t_width+tx];
	cell->c = c;
	cell->fg = fg_color;
	cell->bg = bg_color;
	cell->dirty = 1; tx++;
	if (tx >= t_width) {
		tx = 0; ty++;
	}
}
