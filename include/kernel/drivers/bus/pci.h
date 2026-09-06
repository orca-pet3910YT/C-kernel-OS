#ifndef PCI_H
#define PCI_H
#include <stdint.h>
#include <arch/i386/port.h>
typedef struct {
	uint8_t bus;
	uint8_t slot;
	uint8_t function;
	uint16_t vid;
	uint16_t pid;
	uint16_t command;
	uint16_t status;
	uint8_t revid;
	uint32_t class_code; // 24 bits used
	uint8_t cache_line_s;
	uint8_t lat_timer;
	uint8_t header_type;
	uint8_t bist;
	uint32_t bar0;
	uint32_t bar1;
	uint32_t bar2;
	uint32_t bar3;
	uint32_t bar4;
	uint32_t bar5;
	uint32_t cardbuscisp;
	uint16_t sub_sys_vid;
	uint16_t sub_sys_id;
	uint32_t exprom_ba;
	uint8_t cap_pointer;
	uint8_t int_line;
	uint8_t int_pin;
	uint8_t min_gnt;
	uint8_t max_lat;
} pci_header;
uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
pci_header pci_read_header(uint8_t bus, uint8_t slot, uint8_t func);
uint16_t pci_check_vendor(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_header_type(uint8_t bus, uint8_t slot, uint8_t func);
void pci_check_device(uint8_t bus, uint8_t device);
void pci_check_function(pci_header device);
void pci_check_all_buses();
#endif
