#include <stdint.h>

// simple integer to string (why `a` though, i'm not sure) - returning int in case an error happens
int itoa(uint32_t n, char *str) {
	if (!str) return 1;
	int i = 0;
	if (!n) { // may not be clear, but ! (C NOT) checks for non-zero, not just non-NULL
		str[i] = '0';
		str[++i] = '\0'; // NULL-terminate for safety
		return 0;
	}
	while (n) { // n evaluates to true because anything non-zero is true
		if (i > 64) break;
		str[i++] = (n % 10) + '0'; n /= 10;
	}
	str[i] = '\0';
	int st = 0;
	int end = i-1;
	while (st < end) {
		char d = str[st];
		str[st] = str[end];
		str[end] = d;
		st++; end--;
	}
	return 0;
}

int ftoa(double n, char *str, int p) {
	if (!str) return 1;
	int i = 0;
	if (n < 0) {
		str[i++] = '-';
		n = -n;
	}
	uint32_t ic = (uint32_t)n;
	double f = n-ic;
	char nbuf[16];
	itoa(ic, nbuf);
	int j = 0;
	while (nbuf[j]) {
		str[i++] = nbuf[j++];
	}
	str[i++] = '.';
	for (int k = 0; k < p; k++) {
		f *= 10.0; int d = (int)f;
		str[i++] = d + '0';
		f -= d;
	}
	str[i] = '\0'; return 0;
}

char *htoa(uint32_t n, char *str) {
	char digits[] = "0123456789ABCDEF";
	str[0] = '0'; str[1] = 'x';
	for (int i = 0; i < 8; i++) {
		int shift = (7-i)*4;
		str[i+2] = digits[(n >> shift) & 0xF];
	}
	str[10] = '\0';
	return str;
}
