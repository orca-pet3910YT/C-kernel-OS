#include <math/math.h>

int sqrt(int n) {
	if (n <= 0) return 0;
	int x = n;
	int y = (x+1)/2;
	while (y < x) {
		x = y;
		y = (x+n/x)/2;
	}
	return x;
}

char is_prime(int n) {
	if (n <= 1) return 0;
	if (n == 2) return 1;
	if (!(n % 2)) return 0;
	int max = sqrt(n); // never calculate up to n and instead calculate up to sqrt(n)
	// which saves calculation time
	for (int i = 3; i <= max; i += 2) {
		if (!(n % i)) return 0;
	}
	return 1;
}

int x_pow_2y(int x, int y) {
	int z = y;
	for (; y; y--) x *= x; return !z ? 1 : x;
}
