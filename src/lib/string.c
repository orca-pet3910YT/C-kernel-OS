#include <stddef.h>
#include <string.h>
#include <stdint.h>

int strcmp(const char *a, const char *b) {
	if (!a || !b) { // if the first char is not NUL
		if (a == b) return 0;
		return a ? 1 : -1;
	}
	size_t bytes = 0;
	while (*a != '\0' && *a == *b && bytes < 16777216/32) { a++; b++; bytes++; } // this shows how many chars before a difference occurs
	return (*(unsigned char *)a) - (*(unsigned char *)b);
}

int strncmp(const char *a, const char *b, unsigned int count) {
	if (!a || !b) return -1;
	while (*a && *b && *a == *b && count) { a++; b++; count--; }
	return *a - *b;
}

size_t strlen(const char *a) {
	size_t bytes = 0;
	while (a[bytes] != '\0' && bytes < 16777216/32) { bytes++; } // almost how strncmp and strcmp count except it stops at the NUL byte not a difference
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

char *strncpy(char *to, const char *from, size_t count) {
	size_t i = 0;
	for (i = 0; i < count && from[i]; i++) to[i] = from[i];
	for (; i < count; i++) to[i] = 0;
	return to;
}

void *memset(void *to, int what, unsigned int count) {
	unsigned char *p = (unsigned char *)to;
	while (count--) *p++ = (unsigned char)what;
	return to;
}

void *memmove(void *to, const void *from, size_t size) {
	char *d; const char *s;
	if (to <= from) {
		d = to; s = from;
		while (size--) *d++ = *s++;
	} else {
		d = to;
		d += size;
		s = from;
		d += size;
		while (size--) *--d = *--s;
	}
	return to;
}

/*void *memmove(void *to, const void *from, size_t size) {
	unsigned char *dest = (unsigned char*)to;
	const unsigned char *src = (const unsigned char *)from;
	if (dest == src || !size) return to;
	*if (dest < src) {
		for (size_t i = 0; i < size; i++) dest[i] = src[i];
	} else {
		for (size_t i = size; i; i--) dest[i-1] = src[i-1];
	}*
	if (dest < src) {
		if ((((uintptr_t)dest ^ (uintptr_t)src) & (sizeof(size_t)-1)) == 0) {
			while (size && ((uintptr_t)dest & (sizeof(size_t)-1))) {
				*dest++ = *src++; size--;
			}
			size_t *_dest = (size_t*)dest;
			const size_t *_src = (const size_t*)src;
			while (size >= sizeof(size_t)) {
				*_dest++ = *_src++; size -= sizeof(size_t);
			}
			dest = (unsigned char*)_dest;
			src = (const unsigned char*)_src;
		}
		while (size--) *dest++ = *src++;
	} else {
		dest += size; src += size;
		if ((((uintptr_t)dest ^ (uintptr_t)src) & (sizeof(size_t)-1)) == 0) {
			while (size && ((uintptr_t)dest & (sizeof(size_t)-1))) {
				*--dest = *--src; size--;
			}
			size_t *_dest = (size_t*)dest;
			const size_t *_src = (const size_t*)src;
			while (size >= sizeof(size_t)) {
				*--_dest = *--_src;
				size -= sizeof(size_t);
			}
			dest = (unsigned char*)_dest;
			src = (const unsigned char*)_src;
		}
		while (size--) *--dest = *--src;
	}
	return to;
}*/

__attribute__((target("sse2"))) void *memcpy(void *dest, const void *src, size_t count) {
	char *d = dest;
	const char *s = src;
	while (count--) *d++ = *s++;
	return dest;
}

int memcmp(const void *a, const void *b, size_t size) {
	const unsigned char *a_ = (const unsigned char*)a;
	const unsigned char *b_ = (const unsigned char*)b;
	for (size_t i = 0; i < size; i++) {
		if (a_[i] < b_[i]) return -1;
		if (b_[i] < a_[i]) return 1;
	}
	return 0;
}

int strchr(char *str, char c) { // returns the first occurence of c in str
	int i = 0;
	for (i = 0; str[i] && str[i] != c; i++);
	return i;
}

uint32_t reverse(uint32_t n) { // API for swapping a value cuz ain't no way __builtin_bswap32(n) is readable
	return __builtin_bswap32(n);
}
