#include <stdint.h>
#include <string.h>

static uint32_t pmm_bitmap[32768];
static uint32_t used_blocks = 0;
static uint32_t blocks = 0;
static uint32_t mem_size = 0;

static inline uint32_t pmm_block_count() { return blocks; }
static inline uint32_t get_block_count() { return blocks; }
static inline uint32_t get_free_block_count() { return blocks-used_blocks; }
static inline uint32_t get_memory_size() { return mem_size; }


static inline void reserve_page(uint32_t page_no) {
	uint32_t offset = page_no/32;
	uint32_t shift_value = page_no % 32;
	uint32_t shift = 1U << shift_value;
	pmm_bitmap[offset] |= shift;
}

static inline void free_page(void *addr) {
	uint32_t page = ((uint32_t)addr)/4096;
	if (page >= blocks) return;
	pmm_bitmap[page/32] &= ~(1U << (page % 32));
}

__attribute__((unused)) static inline unsigned char test_page(uint32_t bit) {
	return pmm_bitmap[bit/32] & (1 << (bit % 32));
}

int mmap_first_free() { // find first free region
	for (uint32_t i = 0; i < pmm_block_count()/32; i++) {
		for (int j = 0; j < 32; j++) {
			int bit = 1U << j;
			if (!(pmm_bitmap[i] & bit)) return i*32+j;
		}
	}
	return -1;
}

void pmm_init(uint32_t memory_size) {
	mem_size = memory_size;
	blocks = (get_memory_size()*1024)/4096;
	used_blocks = get_block_count();
	memset(pmm_bitmap, 0xFF, (get_block_count()+7)/8);
}

void pmm_init_region(uint32_t base, uint32_t size) { // mark region as free to use
	int align = base/4096;
	int blocks_loop = size/4096;
	if (!blocks_loop && size) blocks_loop = 1;
	for (; blocks_loop > 0; blocks_loop--) {
		free_page((void*)(align++*4096));
		used_blocks--;
	}
}

void pmm_deinit_region(uint32_t base, uint32_t size) { // mark region as cannot be used as free
	int align = base/4096;
	uint32_t blocks_loop = (base+size+4096)/4096;
	for (uint32_t page = align; page < blocks_loop; page++) {
		reserve_page(page);
		used_blocks++;
	}
}

void *pmm_alloc_block() {
	if (get_free_block_count() <= 0) return 0;
	int frame = mmap_first_free();
	if (frame == -1) return 0;
	reserve_page(frame);
	uint32_t addr = frame * 4096;
	used_blocks++;
	return (void*)addr;
}

void pmm_free_blocks(void *p) {
	free_page(p);
	used_blocks--;
}
