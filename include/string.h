#include <stddef.h>
int strcmp(const char *a, const char *b);
size_t strlen(const char *a);
char *strcat(char *to, const char *from);
char *strcpy(char *to, const char *from);
void *memset(void *to, int what, unsigned int count);
void *memmove(void *to, const void *from, size_t size);
void *memcpy(void *restrict to, const void *restrict from, size_t size);
int memcmp(const void *a, const void *b, size_t size);
// WARNING: deprecated
//int split(char *str, char sep, char *strings[], int stop);
int strchr(char *str, char c);
