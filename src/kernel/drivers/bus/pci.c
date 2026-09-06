#include <drivers/bus/pci.h>
#include <stdint.h>
#include <arch/i386/port.h>
#include <drivers/video/vga.h>

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	uint32_t addr;
	uint32_t lbus = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;
	uint16_t tmp = 0;
	addr = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | 0x80000000);
	outl(0xCF8, addr);
	tmp = inl(0xCFC) >> ((offset & 2)*8) & 0xFFFF;
	return tmp;
}

pci_header pci_read_header(uint8_t bus, uint8_t slot, uint8_t func) {
	pci_header device;
	uint16_t vendor;
	vendor = pci_config_read_word(bus, slot, func, 0);
	device.bus = bus;
	device.slot = slot;
	device.vid = vendor;
	if (vendor != 0xFFFF) {
		device.pid = pci_config_read_word(bus, slot, func, 2);
		device.command = pci_config_read_word(bus, slot, func, 4);
		device.status = pci_config_read_word(bus, slot, func, 6);
		device.revid = pci_config_read_word(bus, slot, func, 8) & 0xFF;
		device.class_code = ((pci_config_read_word(bus, slot, func, 8)*0xFF00) >> 7) & (((uint32_t)pci_config_read_word(bus, slot, func, 10)) << 7);
		device.cache_line_s = pci_config_read_word(bus, slot, func, 12) & 0xFF;
		device.lat_timer = (pci_config_read_word(bus, slot, func, 12) & 0xFF00) >> 7;
		device.header_type = pci_config_read_word(bus, slot, func, 14) & 0xFF;
		device.bist = (pci_config_read_word(bus, slot, func, 14) & 0xFF00) >> 7;
		device.bar0 = pci_config_read_word(bus, slot, func, 16) & ((uint32_t)pci_config_read_word(bus, slot, func, 18) << 15);
		device.bar1 = pci_config_read_word(bus, slot, func, 20) & ((uint32_t)pci_config_read_word(bus, slot, func, 22) << 15);
		device.bar2 = pci_config_read_word(bus, slot, func, 24) & ((uint32_t)pci_config_read_word(bus, slot, func, 26) << 15);
		device.bar3 = pci_config_read_word(bus, slot, func, 28) & ((uint32_t)pci_config_read_word(bus, slot, func, 30) << 15);
		device.bar4 = pci_config_read_word(bus, slot, func, 32) & ((uint32_t)pci_config_read_word(bus, slot, func, 34) << 15);
		device.bar5 = pci_config_read_word(bus, slot, func, 36) & ((uint32_t)pci_config_read_word(bus, slot, func, 38) << 15);
		device.cardbuscisp = pci_config_read_word(bus, slot, func, 40) & ((uint32_t)pci_config_read_word(bus, slot, func, 42) << 15);
		device.sub_sys_vid = pci_config_read_word(bus, slot, func, 44);
		device.sub_sys_id = pci_config_read_word(bus, slot, func, 46);
		device.exprom_ba = pci_config_read_word(bus, slot, func, 48);
		device.cap_pointer = pci_config_read_word(bus, slot, func, 50) & 0xFF;
	}
	return device;
}

void pci_check_device(uint8_t bus, uint8_t device) {
	uint8_t func = 0;
	pci_header dev = pci_read_header(bus, device, 0);
	uint16_t vendor = dev.vid;
	if (vendor == 0xFFFF) return;
	pci_check_function(dev);
	uint8_t header_type = dev.header_type;
	if (header_type & 0x80) {
		for (func = 1; func < 8; func++) {
			dev = pci_read_header(bus, device, func);
			if (dev.vid != 0xFFFF) pci_check_function(dev);
		}
	}
}

void pci_check_function(pci_header dev) {
	printk(6, "pci: %x:%x.%x: vid=%x pid=%x", dev.bus, dev.slot, dev.function, dev.vid, dev.pid);
}

void pci_check_all_buses() {
	uint16_t bus;
	uint8_t dev;
	for (bus = 0; bus < 256; bus++) {
		for (dev = 0; dev < 32; dev++) pci_check_device(bus, dev);
	}
}
