#include <sys/globals.h> // Globals
#include <drivers/video/vga.h> // Display characters and string
#include <drivers/input/kb.h> // Keyboard functions (deprecated outside IDT)
#include <string.h> // String tools
#include <sys/panic.h> // Critical errors
#include <stdlib.h> // Number to string functions
#include <drivers/uart/serial.h> // Serial I/O
#include <drivers/power/power.h> // Power options
#include <arch/i386/port.h> // Port I/O
#include <sys/multiboot.h> // Multiboot info for command line
#include <arch/i386/idt.h> // Interrupt handling
#include <arch/i386/gdt.h> // Required for IDT
#include <drivers/timers/pit.h> // Programmable Interval Timer
#include <arch/i386/pic.h> // PIC
#include <generated/__GENVER.h> // Build info
#include <arch/i386/cpu.h> // CPU vendor string
#include <generated/__BLD.h> // Build number
#include <stddef.h> // Stuff like uint8_t
#include <drivers/video/font.h> // Framebuffer functions
#include <generated/config.h> // Kernel config (obviously)
#include <drivers/audio/pcspkr.h>
#include <math/math.h>
#include <id.h> // kernel_id
#include <fs/cpio.h>
#include <sys/brainfuck.h> // brainfuck_interpret()
#include <fs/vfs.h>
#include <sys/shell.h>

static fb_info_t fb_info_real;
static color_info_t color_info_real;

fb_info_t *fb_info = &fb_info_real;
color_info_t *color_info = &color_info_real;

char *__split_cmdline(char **buffer) {
	char *a, *b;
	a = *buffer;
	if (!a) return NULL;
	for (b = a; *b && strchr(b, ' '); b++);
	if (*b) {
		*b++ = '\0';
		*buffer = b;
	} else {
		*buffer = NULL;
	}
	return a;
}

void parse_cmdline(char *input) {
	char *a; char *b = input;
	while ((a = __split_cmdline(&b))) {
		if (*a == '\0') continue;
		printk(6, "Received command line argument: %s", a);
		if (strcmp(a, "shut_up") == 0) {
			printk(7, "Shh");
			loglevel = 2;
		} else if (strcmp(a, "s_out") == 0) {
			serial_out = true;
			//sputs("\e[0m\e[H\e[J\e[1J\e[2J\r\e[=7h\e[?25h");
			/*
			 * In order:
			 * '\e[0m'            disable all modes
			 * '\e[H'             move cursor to (0, 0)
			 * '\e[J'             erase what's displayed
			 * '\e[1J'            erase from cursor to beginning of screen
			 * '\e[2J'            erase entire screen
			 * '\e[=7h'           enable line wrap
			 * '\e[?25h'          make cursor visible
			 * Thanks to @fnky on GitHub for their Gist on this!
			 */
			printk(6, "Serial output enabled");
		} else if (strcmp(a, "s_in") == 0) {
			serial_in = true;
			printk(6, "Serial input enabled");
		} else {
			printk(2, "Invalid command line argument %s, ignoring", a);
		}
	}
}

extern void _start();
void debug_info_print() {
	printk(6, ver); printk(6, "Build #%d by %s", BUILD_NUMBER, CKOS_BLD);
	printk(6, "Git source tree commit hash: %s", GIT_SOURCE_HASH);
	printk(6, "Kernel entry offset: %x, image offset: %x", _start, 1024*1024);
	printk(6, "Compiler: %s", compiler);
	if (c_version) printk(6, "Compiled with C%d", c_version);
	if (compiler_ver[0] || compiler_ver[1] || compiler_ver[2]) printk(6, "%s %d.%d.%d", compiler, __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
	printk(7, "Made by orca.pet3910YT with %s", "\x03");
}

void vga_old_putc(char c, short loc) {
	((uint16_t*)0xB8000)[loc] = 0x0700 | c;
}

struct cpufreq_s *cpufreq;
char *no_fb_err = "A framebuffer is required to continue. If you have serial, boot logs will go there.";
char command[256] = {0};

/* it is in the license where it says
  If the program does terminal interaction, make it output a short
  notice like this when it starts in an interactive mode
 */
void print_term_license(void) {
	printf("C Kernel OS Copyright (C) 2026 orca-pet3910YT\n");
    	printf("This program comes with ABSOLUTELY NO WARRANTY; for details type 'wlicense'.\n");
    	printf("This is free software, and you are welcome to redistribute it\n");
    	printf("under certain conditions; type 'dlicense' for details.\n\n");
}

void print_term_warranty(void) {
	printf("\tTHERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY\n"
	"APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT\n"
	"HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY\n"
	"OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,\n"
	"THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n"
	"PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM\n"
	"IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF\n"
	"ALL NECESSARY SERVICING, REPAIR OR CORRECTION.\n");
}

void print_term_distrib(void) {
	printf("	This program is free software: you can redistribute it and/or modify\n"
    	"it under the terms of the GNU General Public License as published by\n"
    	"the Free Software Foundation, either version 3 of the License, or\n"
    	"(at your option) any later version.\n"
    	"This program is distributed in the hope that it will be useful,\n"
    	"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    	"GNU General Public License for more details.\n"
    	"You should have received a copy of the GNU General Public License\n"
    	"along with this program.  If not, see <https://www.gnu.org/licenses/>.\n");
}

void _Noreturn kmain(int magic, uint32_t *mbi) {
	(void)magic;
	//volatile char* video = (volatile char*)0xB8000;
	//video[0] = 'E';
	//video[1] = 0x07;
	//putc('h');
	//putc('\n');
	//putc('i');
	//char string_thing[12] = {0};
	//ftoa(0.25, string_thing);
	//puts(string_thing);
	/*
	 * do not even attempt to remove the comments above.
	 */
	set_post(0x3E);
	uint32_t *mbi_old = mbi;
	serial_init();
	kernel_id->major = CONFIG_KERNEL_MAJOR;
	kernel_id->minor = CONFIG_KERNEL_MINOR;
	kernel_id->patch = CONFIG_KERNEL_PATCH;
	kernel_id->additional_user = CONFIG_ADDITIONAL_VER;
	kernel_id->additional_makefile = CONFIG_KERNEL_ADDITIONAL_MAKEFILE;
	char *cmdline = NULL;
	char can_font_init = 0;
	uint8_t *ptr = (uint8_t*)mbi;
	uint32_t mbi_size = *mbi;
	ptr += 8;
	// while the current tag pointer isn't past the end
	while (ptr < (uint8_t*)mbi+mbi_size) {
		printk(7, "tag type %d size %d ptr %x", *(uint32_t*)ptr, *(uint32_t*)(ptr+4), ptr);
		if (!*(uint32_t*)ptr) break;
		if (*(uint32_t*)ptr == 8) {
			// set up framebuffer
			fb_info->fb = (uint32_t*)(uintptr_t)(*(uint64_t*)(ptr+8));
			fb_info->pitch = *(uint32_t*)(ptr+16);
			fb_info->w = *(uint32_t*)(ptr+20);
			fb_info->h = *(uint32_t*)(ptr+24);
			fb_info->bpp = *(uint8_t*)(ptr+28);
			if (fb_info->bpp == 32) can_font_init = 1;
			// initialize framebuffer from data above
			fb_init(fb_info, can_font_init);
			draw_logo(fb_info);
		} else if (*(uint32_t*)ptr == 1) {
#ifndef CONFIG_CMDLINE_STR
#if !CONFIG_CMDLINE
			cmdline = (char*)(ptr+8);
#else
#error "possible config corruption"
#endif
#else
			cmdline = CONFIG_CMDLINE_STR;
#endif
			parse_cmdline(cmdline);
		} else if (*(uint32_t*)ptr == 2) {
			printk(6, "Booting via %s", (char*)(ptr+8));
			kernel_id->bootloader = (char*)(ptr+8);
		} else if (*(uint32_t*)ptr == 14) {
			unsigned char rsdp_sign[9] = {0};
			memcpy(rsdp_sign, ptr+8, 8);
			printk(6, "RSDP signature: '%s', revision %d, RSDP at %x", &rsdp_sign, (uint32_t)*(ptr+23), *(uint32_t*)(ptr+24));
		}
		ptr += (*(uint32_t*)(ptr+4)+7) & ~7;
	}
	printk(6, "---BEGIN Command line info---");
	parse_cmdline(cmdline);
	printk(4, "Parsed command line provided by bootloader");
	printk(6, "--- END Command line info ---");

	printk(0, "Hello, hello!");
	printk(0, "%x %x %x %x", (uint32_t)mbi_old, (uint32_t)mbi, (uint32_t)ptr, (uint32_t)can_font_init);
	gdt_init();
	printk(6, "GDT initialized");
	pic_remap();
	printk(6, "Remapped the PIC");
	init_idt();
	printk(6, "IDT initialized");
	init_pit();
	printk(6, "Initialized PIT at 10 KHz");
	__asm__ volatile ("sti");
	printk(6, "Set interrupts");
	printk(7, "isr6(): %x", isr6);
	printk(7, "isr0(): %x", isr0);

	debug_info_print();
	printk(5, "Command line: %s", cmdline);
	printk(6, "Initialized serial at %x (COM1) and %x (COM2)", UART1, UART2);
	
	if (serial_out) {
		printk(4, "Printing framebuffer info for serial console");
		fb_debug_print(fb_info);
	}

	kb_init();
	printk(6, "Initialized PS/2 BIOS keyboard");
	printk(7, "CPU: %s", get_cpu_vendor());
	// get_cpu_vendor in cpu.asm is a simple assembly function that uses cpuid with eax = 0
	// which puts the max eax value in eax and the CPU vendor in ebx, edx and ecx in order.
	// technically i don't think you have to call it more than once but never too safe :P
	char brand[13];
	printk(7, "---> %s", get_cpu_brand(brand));
	volatile uint32_t pre_test_time = uptime_ticks;
	for (int i = 0; i < 10000000; i++) __asm__ volatile ("data16 ds cs nopl 0x0(%eax, %eax, 1)");
	volatile uint32_t post_test_time = uptime_ticks;
	printk(4, "CPU test passed, took %d ms for a 10 million NOPL repetitions loop", (post_test_time-pre_test_time)/10);
	uint32_t __uptimeticks = (uint32_t)uptime_ticks;
	pre_test_time = uptime_ticks;
	for (int i = 0; i < 10000000; i++) {
		__uptimeticks = (uint32_t)uptime_ticks;
		__asm__ volatile ("xorl %[pattern], %[ticks]" : [ticks] "+r" (__uptimeticks) : [pattern] "r" (i) : "cc");
	}
	post_test_time = uptime_ticks;
	printk(4, "CPU test passed, took %d ms for a 10 million XOR repetitions loop", (post_test_time-pre_test_time)/10);
	printk(4, "square root of 4 is %d and of 9 is %d", sqrt(4), sqrt(9));
	printk(4, "4 is%s a prime, 0x7FFFFFFF is%s a prime and 13 is%s a prime", is_prime(4) ? "" : " not", is_prime(0x7FFFFFFF) ? "" : " not", is_prime(13) ? "" : " not");
	printk(4, "testing primes from 1 to 100000");
	pre_test_time = uptime_ticks;
	for (int i = 1; i < 100000; i++) (void)(is_prime(i));
	post_test_time = uptime_ticks;
	printk(4, "CPU test passed, took %d ms for a 100 thousand prime number calculation loop", (post_test_time-pre_test_time)/10);
	
	printk(4, "hello/hello.txt with cwd /home/user: %s", resolve_path("hello/hello.txt", "/home/user"));
	init_cpio();
	printk(4, "Initialized cpio filesystem");
	file_t init = read("/init", 5);
	printk(4, "Run /init (%d bytes)", init.size);
	brainfuck_interpret(init.data, init.size);
	printk(4, "write code %d", write(&init, "Hello, World!", 13));
	file_t welcome_banner = read("/welcome", -1);
	if (welcome_banner.data) print(welcome_banner.data, welcome_banner.size);
	printk(7, "Hello, World!");
#ifdef CONFIG_LOGO
#if CONFIG_LOGO
	uint32_t color = fg_color;
	set_color(bg_color, 0x00FFFFFF);
	printf("%s\n", logo); // globals.h:4
	set_color(bg_color, color);
#endif
#endif
	printk(0, "Welcome to CkOS!");
	set_post(0x00);
	kernel_boot_ticks = uptime_ticks;
	//panic("This PC is ass."); // compile with this uncommented to prank people :)
	printf("Running shell\n");

	shell_init();
	while (1) {
		shell_cmd_loop();
		/* do stuff */
	}
	printk(4, "Halting because we have nothing to do.");
	halt();
}
