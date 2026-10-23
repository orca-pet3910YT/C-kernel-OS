#ifndef GDT_H
#define GDT_H
typedef struct {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	union {
		uint8_t access;
		struct {
			uint8_t accessed : 1;
			uint8_t read_write : 1;
			uint8_t conforming_expand_down : 1;
			uint8_t code : 1;
			uint8_t code_data_segment : 1;
			uint8_t dpl : 2;
			uint8_t present : 1;
		} __attribute((packed));
	};
	union {
		uint8_t granularity;
		struct {
			uint8_t limit_high : 4;
			uint8_t available : 1;
			uint8_t long_mode : 1;
			uint8_t big : 1;
			uint8_t gran : 1;
		} __attribute__((packed));
	};
	uint8_t base_high;
} __attribute__((packed)) gdt_ent_t;

typedef gdt_ent_t gdt_entry_bits; // compatibility with tss code

typedef struct {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

void gdt_init();
#endif
