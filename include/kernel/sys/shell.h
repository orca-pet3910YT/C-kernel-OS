/* Copyright (C) GPLv3 2026 JM-Pilot */

#ifndef SHELL_H
#define SHELL_H

/* initialize the kernel shell */
void shell_init(void);

/* get command and parse it */
void shell_cmd_loop(void);

/* prints the license */
void print_term_license(void);

/* prints the warranty details */
void print_term_warranty(void);

/* prints the distribution guide */
void print_term_distrib(void);
#endif