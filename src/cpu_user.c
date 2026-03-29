#include <cpu.h>
#include <string.h>
#include <cpuid.h> // first ever header outside CkOS to be included!

char *get_cpu_vendor_user() {
	char *vendor = get_cpu_vendor();
	if (strcmp(vendor, "GenuineIntel") == 0) {
		return "Intel x86";
	} else if (strcmp(vendor, "AuthenticAMD") == 0) {
		return "AMD x86";
	} else if (strcmp(vendor, "GenuineIotel") == 0) {
		return "Intel ???";
	} else if (strcmp(vendor, "CentaurHalls") == 0) {
		return "IDT WinChip/Centaur";
	} else if (strcmp(vendor, "CyrixInstead") == 0) {
		return "Cyrix";
	} else if (strcmp(vendor, "TransmetaCPU") == 0 || strcmp(vendor, "GenuineTMx86") == 0) {
		return "Transmeta";
	} else if (strcmp(vendor, "Geode by NSC") == 0) {
		return "National Semiconductor";
	} else if (strcmp(vendor, "NexGenDriven") == 0) {
		return "NexGen";
	} else if (strcmp(vendor, "RiseRiseRise") == 0) {
		return "Rise x86";
	} else if (strcmp(vendor, "SiS SiS SiS ") == 0) {
		return "Silicon Integrated Systems";
	} else if (strcmp(vendor, "UMC UMC UMC ") == 0) {
		return "United Microelectronics Corp.";
	} else if (strcmp(vendor, "Vortex86 SoC") == 0) {
		return "Vortex86";
	} else if (strcmp(vendor, "  Shanghai  ") == 0) {
		return "Zhaoxin";
	} else if (strcmp(vendor, "HygonGenuine") == 0) {
		return "Hygon";
	} else if (strcmp(vendor, "Genuine  RDC") == 0) {
		return "RDC Semiconductor";
	} else if (strcmp(vendor, "E2K MACHINE ") == 0) {
		return "Elbrus";
	} else if (strcmp(vendor, "VIA VIA VIA ") == 0) {
		return "VIA";
	} else if (strcmp(vendor, "AMD ISBETTER") == 0) {
		return "AMD K5 (early)";
	}
	return "Unknown";
}

char *get_cpu_brand(char buffer[]) {
	unsigned int r[12];
	__cpuid(0x80000000, r[0], r[1], r[2], r[3]);
	if (r[0] < 0x80000004) return "Unknown";
	__cpuid(0x80000002, r[0], r[1], r[2], r[3]);
	__cpuid(0x80000003, r[4], r[5], r[6], r[7]);
	__cpuid(0x80000004, r[8], r[9], r[10], r[11]);
	memcpy(buffer, r, sizeof(r));
	buffer[sizeof(r)] = '\0';
	return buffer;
}
