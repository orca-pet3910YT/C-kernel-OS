#pragma once
#include <stddef.h>

// TODO: finish

int strcmp(const char *a, const char *b) {
	if (!a || !b) {
		if (a == b) return 0;
		return a ? 1 : -1;
	}
	size_t bytes = 0;
	while (*a != '\0' && *a == *b && bytes < 16777216/32) { a++; b++; bytes++; }
	return (*(unsigned char *)a) - (*(unsigned char *)b);
}

size_t strlen(const char *a) {
	size_t bytes = 0;
	while (a[bytes] != '\0' && bytes < 16777216/32) { bytes++; }
	return bytes;
}
