#include <sys/multiboot.h>
#include <drivers/video/vga.h>
#include <stdint.h>
#include <sys/globals.h>
#include <drivers/timers/pit.h>
#include <drivers/video/font.h>
#include <sys/panic.h>
#include <generated/config.h>
#include <arch/i386/pmm.h>

static fb_info_t framebuffer_info_real;
fb_info_t *framebuffer_info = &framebuffer_info_real;

static inline void _set_pixel(fb_info_t *fb_info, int x, int y, uint32_t color) {
	uint32_t *row = (uint32_t*)((uint8_t*)fb_info->fb+y*fb_info->pitch);
	row[x] = color & 0x00FFFFFF;
}

void set_pixel(int x, int y, unsigned int color) {
	if ((unsigned)x >= framebuffer_info->w || (unsigned)y >= framebuffer_info->h || x < 0 || y < 0) return;
	_set_pixel(framebuffer_info, x, y, color);
}

// this demo is garbage, don't use it :)

/*void fb_demo_1(fb_info_t *fbi) {
	uint32_t *framebuffer = (uint32_t*)(uintptr_t)fbi->fb;
	printk(6, "Beginning filling the screen with gray...");
	for (unsigned int i = 0; i < fbi->w*fbi->h*(fbi->bpp/8); i++) {
		framebuffer[i] = 0x00808080;
	}
	printk(6, "Done!");
}*/

/*void fb_demo_2(fb_info_t *fbi) {
	printk(4, "Press 'q' to exit");
	printk(6, "Drawing pattern...");
	for (int o = 0; o < 1000000; o++) {
		for (unsigned int i = 0; i < fbi->w; i++) {
			for (unsigned int j = 0; j < fbi->h; j++) {
				set_pixel(i, j, (0x00000A00 | (((i-j+(o*15))) & 0x7F) << (o % 24)));
			}
		}
		if (kbc) {
			if (kbc == 'q') {
				kbc = 0;
				clear_screen(fbi);
				printk(6, "User-requested exit.");
				return;
			} else {
				kbc = 0;
			}
		}
	}
	clear_screen(fbi);
	printk(6, "Done!");
}*/
#ifdef CONFIG_ANIMATIONS
#if CONFIG_ANIMATIONS
static inline uint32_t f(uint32_t x, uint32_t y, int o) {
	return ((x-y+o/5) & 0x7F) << (o%24);
}

void fb_demo_2(fb_info_t *fbi) {
	printk(4, "Press 'q' to exit");
	uint32_t w = fbi->w;
	uint32_t h = fbi->h;
	uint32_t pitch = fbi->pitch;
	uint8_t *fb = (uint8_t*)fbi->fb;
	printk(6, "Drawing pattern...");
	for (int o = 0; o < 1000000; o++) {
		//int b = o*15; int s = o & 23;
		for (uint32_t y = 0; y < h; y++) {
			uint32_t *row = (uint32_t*)(fb+y*pitch);
			for (uint32_t x = 0; x < w; x++) {
				//row[x] = 0x00000A00 | (((x - y + b) & 127) << s);
				//set_pixel(x, y, 0x00000A00 | f(x, y, o/3));
				row[x] = 0x00000A00 | f(x, y, o/25);
			}
		}
		if (kbc) {
			if (kbc == 'q') {
				kbc = 0;
				clear_screen();
				printk(6, "User-requested exit.");
				return;
			} else {
				kbc = 0;
			}
		}
	}
	clear_screen();
	printk(6, "Done!");
}

/*static inline uint32_t g(uint32_t x, uint32_t y, int o) {
	//for (int i = 0; i < 5; i++) __asm__ volatile ("cpuid" : : : "eax", "ebx", "ecx", "edx");
	return (x-y+o) << 16;
}*/

void fb_demo_3(fb_info_t *fbi) {
	printk(4, "Press 'q' to exit");
	uint32_t w = fbi->w;
	uint32_t h = fbi->h;
	uint8_t *fb = (uint8_t*)fbi->fb;
	uint32_t pitch = fbi->pitch;
	printk(6, "Drawing pattern...");
	for (int o = 0; o < 1000000; o++) {
		for (uint32_t y = 0; y < h; y++) {
			uint32_t *row = (uint32_t*)(fb+y*pitch);
			uint32_t b = (o-y) << 16;
			for (uint32_t x = 0; x < w; x++) {
				//set_pixel(x, y, 0x00000A00 | g(x, y, o));
				row[x] = b+(x << 16);
			}
		}
		/*for (int i = 0; i < 500000; i++) {
			volatile int t = (2 & 5) ^ 1;
			t = (o ^ w) / ((t / 4) + 1);
			t /= o + (t ^ (w / (h + 1))) + 1;
			t ^= (i * (o+1)) / 15;
			t &= 15;
		}*/
		if (kbc) {
			if (kbc == 'q') {
				kbc = 0;
				clear_screen();
				printk(6, "User-requested exit.");
				return;
			}
			kbc = 0;
		}
	}
	clear_screen();
	printk(6, "Done!");
}

static uint32_t h(uint32_t x, uint32_t y, uint32_t r) {
	uint32_t p = 0;
	if (y < r) p = r-y;
	else if (y > r*2) p = 0;
	else if (y > r) p = y-r;
	//else if (x == r) p = x;
	else p = 0;
	if (p == x && y < (r*2+1)) return 0x00FFFFFF;
	return 0x00000000;
}

static uint32_t h_(uint32_t x, uint32_t y, uint32_t r) {
	return h(r*3-x, y, r);
}

void fb_demo_4(fb_info_t *fbi) {
	printk(4, "Press 'q' to exit");
	uint32_t w = fbi->w;
	uint32_t ht = fbi->h;
	uint8_t *fb = (uint8_t*)fbi->fb;
	uint32_t pitch = fbi->pitch;
	uint16_t smaller = fbi->h < fbi->w ? fbi->h : fbi->w;
	printk(6, "Drawing pattern...");
	for (uint32_t y = 0; y < ht; y++) {
		uint32_t *row = (uint32_t*)(fb+y*pitch);
		for (uint32_t x = 0; x < w; x++) {
			//set_pixel(x, y, 0x00000A00 | g(x, y, o));
			row[x] = (h(x, y, smaller/2-1) == 0x00FFFFFF ? 0x00FFFFFF : h_(x+smaller/2-1, y, smaller/2-1));
		}
	}
		/*for (int i = 0; i < 500000; i++) {
		 *		volatile int t = (2 & 5) ^ 1;
		 *		t = (o ^ w) / ((t / 4) + 1);
		 *		t /= o + (t ^ (w / (h + 1))) + 1;
		 *		t ^= (i * (o+1)) / 15;
		 *		t &= 15;
	}*/
	for (;;) {
		if (kbc) {
			if (kbc == 'q') {
				kbc = 0;
				clear_screen();
				printk(6, "User-requested exit.");
				return;
			} else {
				kbc = 0;
			}
		}
		__asm__ volatile ("hlt");
	}
	clear_screen();
	printk(6, "Done!");
}

#else
void fb_demo_2(fb_info_t *fbi) {
	(void)fbi;
	printk(2, "Animation code was disabled, if that was a mistake, please recompile with animations on");
}
void fb_demo_3(fb_info_t *fbi) {
	(void)fbi;
	printk(2, "Animation code was disabled, if that was a mistake, please recompile with animations on");
}
void fb_demo_4(fb_info_t *fbi) {
	(void)fbi;
	printk(2, "Animation code was disabled, if that was a mistake, please recompile with animations on");
}
#endif
#endif /* CONFIG_ANIMATIONS */

void fb_init(fb_info_t *fbi, char can_font_init) {
	framebuffer_info = fbi;
	uint32_t *framebuffer = 0;
	(void)framebuffer;
	printk(5, "Size: %dx%dx%d", fbi->w, fbi->h, fbi->bpp);
	if (fbi->bpp != 32) panic("Incorrect framebuffer type");
	framebuffer = fbi->fb; //(uint8_t*)(uintptr_t)fb;
	printk(4, "Framebuffer info found!");
	printk(6, "Initialized framebuffer");
	if (can_font_init) {
		font_init();
		font_initialized = 1;
	}
#if !MEMORY_SECRET
	printk(6, "Framebuffer is at %x", fbi->fb);
#endif
	printk(6, "Size: %dx%dx%d", fbi->w, fbi->h, fbi->bpp);
	if (font_initialized) printk(6, "Framebuffer font system initialized");
	else printk(6, "Framebuffer font system NOT initialized");
	return;
}

#if CONFIG_GLOGO
extern uint8_t _binary_bin_logo_raw_start[];
void draw_logo(fb_info_t *fbi) {
	int startw = fbi->w-256;
	uint32_t *pixels = (uint32_t*)_binary_bin_logo_raw_start;
	for (int w_index = 0; w_index < 256; w_index++) {
		for (int h_index = 0; h_index < 256; h_index++) {
			((uint32_t*)fbi->fb)[h_index*fbi->w+(startw+w_index)] = pixels[h_index*256+w_index];
		}
	}
}
#else
void draw_logo(fb_info_t *fbi) {
	(void)fbi;
}
#endif

void fb_debug_print(fb_info_t *fbi) {
#if !CONFIG_MEMORY_SECRET
	printk(6, "Framebuffer is at %x", fbi->fb);
#endif
	printk(6, "Size: %dx%dx%d", fbi->w, fbi->h, fbi->bpp);
	if (font_initialized) printk(6, "Framebuffer font system initialized");
	else printk(6, "Framebuffer font system NOT initialized");
}
