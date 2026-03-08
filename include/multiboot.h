#ifndef MULTIBOOT_H
#define MULTIBOOT_H
#include <stdint.h>
typedef struct multiboot_info {
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mod_count;
	uint32_t mod_addr;
	uint32_t syms[4];
	uint32_t memmap_length;
	uint32_t memmap_addr;
	uint32_t drive_length;
	uint32_t drive_addr;
	uint32_t cfg_table;
	uint32_t bootloader;
	uint32_t apm_table;
} __attribute__((packed)) mbinfo_t;

typedef struct {
	uint32_t size;
	uint64_t addr;
	uint64_t len;
	uint32_t type;
} __attribute__((packed)) mb_memmap_t;
#endif
