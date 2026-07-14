/* Copyright (C) GPLv3 2026 JM-Pilot */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <drivers/input/kb.h>
#include <drivers/video/vga.h>
#include <sys/globals.h>
#include <sys/shell.h>
#include <drivers/video/font.h>
#include <drivers/power/power.h>
#include <sys/panic.h>
#include <drivers/audio/pcspkr.h>
#include <drivers/timers/pit.h>
#include <generated/config.h>
#include <sys/multiboot.h>
#define BUFFER_WIDTH 2048
/* if true the typed word only shows after the user does EOF or a newline
 * if false the typed word shows directly
 */
static bool canonical_md = false;
static char cmd_buffer[BUFFER_WIDTH];

void shell_init(void)
{
	clear_screen(); // comment this line if you dont want to clear
	printf("CkOS Kernel Shell, type 'help' for more info\n");
}

void shell_cmd_loop(void)
{
	memset(cmd_buffer, 0, sizeof(cmd_buffer));
	printf("$ ");
	char c = 0;
	int cmdb_ptr = 0;
	/* get output first*/
	while (true) {
		while (kbc == 0);
		c = kbc;
		kbc = 0; 
		if (c == '\n') {
			/* null end it */
			cmd_buffer[cmdb_ptr] = '\0';
			break;
		}
		/* skip so we cannot access the random data */
		if (c == '\b') {
			if (cmdb_ptr == 0) continue;
			if (!canonical_md) {
				printf("\b");
			}
			cmdb_ptr--;
			continue;
		}
		/* do non canonical */
		if (!canonical_md) {
			printf("%c", c);
		}
		if (cmdb_ptr < (int)sizeof(cmd_buffer) - 1) {
			cmd_buffer[cmdb_ptr++] = c;
		} else {
			printf("TO MUCH CHARACTERS\n");
		}
	}
	/* do canonical */
	if (canonical_md) {
		for (int i = 0; cmd_buffer[i] != '\0'; i++)
			printf("%c", cmd_buffer[i]);
	}
	printf("\n");

	if (strcmp(cmd_buffer, "help") == 0) {
		print_term_license();
		printf("Available Commands\n"
		        "hello    -- greet the world!\n"
			"poweroff -- shutdown the pc (QEMU ONLY)\n"
			"reboot   -- reboot the pc (QEMU ONLY)\n"
			"halt     -- halts the pc (non recoverable)\n"
			"echo     -- echoes the string you typed\n"
			"credits  -- show credits\n"
			"help     -- show this menu\n"
			"clear    -- clears the screen\n"
			"panic    -- panic test (non recoverable)\n"
			"logo     -- prints the logo\n"
			"uptime   -- shows the uptime\n"
			"beep     -- makes the speaker go beep\n"
			"square   -- draws 2 square\n"
			"setcan   -- sets canonical_md to true\n"
			"setncan  -- sets canonical_md to false\n"
		);
	} 

	/* hello */
	else if (strcmp(cmd_buffer, "hello") == 0) {
		printf("Hello World!\n");
	} 

	/* credits*/
	else if (strcmp(cmd_buffer, "credits") == 0) {
		printf(credits);
	}
	
	/* clear */
	else if (strcmp(cmd_buffer, "clear") == 0) {
		clear_screen();
	}

	/* poweroff */
	else if (strcmp(cmd_buffer, "poweroff") == 0) {
		poweroff();
		panic("FAILED TO REBOOT");
	}

	/* reboot */
	else if (strcmp(cmd_buffer, "reboot") == 0) {
		reboot();
		panic("FAILED TO REBOOT");
	}

	/* halt */
	else if (strcmp(cmd_buffer, "halt") == 0) {
		halt();
	}

	/* dlicense */
	else if (strcmp(cmd_buffer, "dlicense") == 0) {
		print_term_distrib();
	}

	/* wlicense */
	else if (strcmp(cmd_buffer, "wlicense") == 0) {
		print_term_warranty();
	}

	/* panic */
	else if (strcmp(cmd_buffer, "panic") == 0) {
		panic("PANIC TEST :^)");
	}

	/* echo */
	else if (strncmp(cmd_buffer, "echo ", 5) == 0) {
		printf("%s\n", cmd_buffer + 5);
	}

	/* logo */
	else if (strcmp(cmd_buffer, "logo") == 0) {
		set_color(0x00000000, 0x00FFFFFF);
 		printf("%s\n", logo);
		set_color(0x00000000, 0x00AAAAAA);
	}

	/* uptime */
	else if (strcmp(cmd_buffer, "uptime") == 0) {
		uint32_t uptime_secs = (uint32_t)(uptime_ticks)/10000; // src/pit.c
		printf("Uptime in seconds: %d\n", uptime_secs);
		uint32_t uptime_minutes = uptime_secs/60;
		uint32_t uptime_hours = uptime_minutes/60;
		uint32_t uptime_days = uptime_hours/24;
		uint32_t uptime_years = uptime_days/365;
		if (uptime_years == 1) {
			printf("1 year, ");
		} else if (uptime_years > 1) {
			printf("%d years, ", uptime_years);
		}
		if (uptime_days == 1) {
			printf("1 day, ");
		} else if (uptime_days > 1) {
			printf("%d days, ", uptime_days % 365);
		}
		if (uptime_hours == 1) {
			printf("1 hour, ");
		} else if (uptime_hours > 1) {
			printf("%d hours, ", uptime_hours % 24);
		}
		if (uptime_minutes == 1) {
			printf("1 minute and ");
		} else if (uptime_minutes > 1) {
			printf("%d minutes and ", uptime_minutes % 60);
		}
		printf("%d seconds\n", uptime_secs % 60);
	}

	/* beep */
	else if (strcmp(cmd_buffer, "beep") == 0) {
				set_pcspkr_frequency(440);
				delay(250);
				no_pcspkr();
	} 
	/* square */
	else if (strcmp(cmd_buffer, "square") == 0) {
		clear_screen();
		draw_rect(150, 150, 150, 150, 0x00FFFFFF);
		delay(3000);
		clear_screen();
		draw_rect_outline(100, 100, 125, 125, 0x00FFFFFF, 5);
		delay(3000);
		clear_screen();
	}

	/* canonical md control */
	else if (strcmp(cmd_buffer, "setcan") == 0) {
		canonical_md = true;
	}
	else if (strcmp(cmd_buffer, "setncan") == 0) {
		canonical_md = false;
	}

	else {
		printf("Invalid command, type help to show the right commands\n");
	}
}

/* it is in the license where it says
  If the program does terminal interaction, make it output a short
  notice like this when it starts in an interactive mode
 */
void print_term_license(void)
{
	printf("C-Kernel-OS Copyright (C) 2026 orca-pet3910YT\n");
    	printf("This program comes with ABSOLUTELY NO WARRANTY; for details type 'wlicense'.\n");
    	printf("This is free software, and you are welcome to redistribute it\n");
    	printf("under certain conditions; type 'dlicense' for details.\n\n");
}

void print_term_warranty(void)
{
	printf("     THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY\n"
	"APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT\n"
	"HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY\n"
	"OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,\n"
	"THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n"
	"PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM\n"
	"IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF\n"
	"ALL NECESSARY SERVICING, REPAIR OR CORRECTION.\n");
}

void print_term_distrib(void)
{
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