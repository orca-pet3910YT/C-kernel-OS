#include <sys/globals.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <generated/config.h>
double uptime = 0.0;
//const char *ver = "CkOS Beta Version 0.03 for i386 (x86-32) at 2nd April 2026";
const char *ver = CONFIG_KERNEL_VER_STR;
/*const char *logo =
" _   _  _\n"
"/ |/| ||_`\n"
"\\_|\\|_|._|";*/
const char *logo =
" __________\n"
"|       _\n"
"| |/ /\\|_`\n"
"| |\\ \\/._|\n"
"|__________\n";
const char *credits =
"Credits:\n"
"        axrxvm: numerous helpful resources about aOS\n"
"      oslayout: gave me a copy of PotatoOS for studying\n"
"           you: for usage of this experimental OS\n"
"      orca-pet3910YT: creator\n"
"      JM-Pilot: contributor\n";
char serial_in = false; // true and false is defined in stdlib.h. while this is not the C standard, it makes you have to import less headers.
char serial_out = false;
char serial_com1 = true;
char serial_com2 = true;
volatile char kbc;
// i'm sorry fellow C developers for this shitty solution but i just have to do this
volatile uint64_t uptime_ticks = 0;

#ifdef CONFIG_LOGLEVEL
#if CONFIG_LOGLEVEL <= 7 && CONFIG_LOGLEVEL >= 0
unsigned char loglevel = CONFIG_LOGLEVEL; // You don't want to recompile the kernel just because you don't have all the info to debug an error
#else
#error "invalid loglevel, must be positive (or zero) and below 7"
#endif
#else
unsigned char loglevel = 7;
#endif /* CONFIG_LOGLEVEL */

char font_initialized = 0;
int fb_x = 0; int fb_y = 0;
#ifdef CONFIG_COLOR_FB_FG
uint32_t fg_color = CONFIG_COLOR_FB_FG;
#else
uint32_t fg_color = 0x00AAAAAA;
#endif
#ifdef CONFIG_COLOR_FB_BG
uint32_t bg_color = CONFIG_COLOR_FB_BG;
#else
uint32_t bg_color = 0x00000000;
#endif
