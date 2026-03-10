#include <globals.h>
#include <stdlib.h>
double uptime = 0.0;
const char *ver = "CkOS Beta Version 0.02 for i386 (x86-32) at 10th March 2026";
const char *logo =
" _   _  _\n"
"/ |/| ||_`\n"
"\\_|\\|_|._|";
const char *credits =
"Credits:\n"
"        axrxvm: numerous helpful resources about aOS\n"
"      oslayout: gave me a copy of PotatoOS for studying\n"
"           you: for usage of this experimental OS";
bool serial_in = false; // true and false is defined in stdlib.h. while this is not the C standard, it makes you have to import less headers.
bool serial_out = false;
volatile char kbc = 0;
