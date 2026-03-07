#include <stddef.h>
#include <string.h>

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

char *strcat(char *to, const char *from) {
	if (!to || !from) return to;
	char *d = to;
	size_t i = 0;
	while (*d != '\0' && i < 16777216/32) {
		d++; i++;
	}
	if (i >= 16777216/32) return to;
	i = 0;
	while ((*d++ = *from++) != '\0' && i < 16777216/32) i++;
	if (i >= 16777216/32) *(d-1) = '\0';
	return to;
}

char *strcpy(char *to, const char *from) {
	if (!to || !from) return to;
	char *d = to;
	size_t i = 0;
	while ((*d++ = *from++) != '\0' && i < 16777216/32) i++;
	if (i >= 16777216/32) *(d-1) = '\0';
	return to;
}

void *memset(void *to, int what, unsigned int count) {
	unsigned char *p = (unsigned char *)to;
	while (count--) *p++ = (unsigned char)what;
	return to;
}

void *memmove(void *to, const void *from, size_t size) {
	unsigned char *dest = (unsigned char*)to;
	const unsigned char *src = (const unsigned char *)from;
	if (dest < src) {
		for (size_t i = 0; i < size; i++) dest[i] = src[i];
	} else {
		for (size_t i = size; i; i--) dest[i-1] = src[i-1];
	}
	return to;
}

void *memcpy(void *restrict to, const void *restrict from, size_t size) {
	unsigned char *dest = (unsigned char*)to;
	const unsigned char *src = (const unsigned char*)from;
	for (size_t i = 0; i < size; i++) dest[i] = src[i];
	return to;
}

int memcmp(const void *a, const void *b, size_t size) {
	const unsigned char *a_ = (const unsigned char*) a;
	const unsigned char *b_ = (const unsigned char*) b;
	for (size_t i = 0; i < size; i++) {
		if (a_[i] < b_[i]) return -1;
		if (b_[i] < a_[i]) return 1;
	}
	return 0;
}
