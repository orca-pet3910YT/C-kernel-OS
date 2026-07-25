#ifndef PMM_H
#define PMM_H
#include <stdint.h>
void pmm_init(uint32_t mem_size);
void pmm_init_region(uint32_t base, uint32_t size);
void pmm_deinit_region(uint32_t base, uint32_t size);
void *pmm_alloc_block();
void pmm_free_blocks(void *p);
int mmap_first_free();
#endif
