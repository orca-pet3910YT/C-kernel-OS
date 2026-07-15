#ifndef GLOBALS
#define GLOBALS
#include <stdlib.h>
#include <stdint.h>
extern double uptime;
extern const char *ver;
extern const char *logo;
extern const char *compiler;
extern const int compiler_ver[3];
extern const int c_version;
extern char serial_in;
extern char serial_out;
extern char serial_com1;
extern char serial_com2;
extern volatile char kbc;
extern const char *credits;
extern volatile uint64_t uptime_ticks;
extern unsigned char loglevel;
extern char font_initialized;
extern int tx;
extern int ty;
extern uint32_t fg_color;
extern uint32_t bg_color;
extern uint64_t kernel_boot_ticks;
extern uint32_t drv_dbg[8];

// Print levels
#define PR_EMERG 0
#define PR_CRIT 1
#define PR_ERROR 2
#define PR_WARN 3
#define PR_INFO 4
#define PR_DETAIL 5
#define PR_DEBUG 6
#define PR_UNNECESSARY 7
#endif
