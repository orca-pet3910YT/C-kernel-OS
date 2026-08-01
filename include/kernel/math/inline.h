#ifndef MATH_INLINE_H
#define MATH_INLINE_H
#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))
#define PI 3.14159265358979323846 // apple pie
#define abs(n) ((n) < 0 ? (-n) : (n)) // |n|
static inline __attribute__((always_inline)) double sin(double n) {
	n = n*PI/180.0;
	while (n > PI) n -= 2*PI;
	while (n < -PI) n += 2*PI;
	double output;
	__asm__ volatile (
		"fldl %1;"
		"fsin;"
		"fstpl %0;"
		: "=m"(output) // output
		: "m"(n) // input
		: "st" // FPU stack
	);
	return output;
}
static inline __attribute__((always_inline)) double cos(double n) {
	return sin(n+90.0);
}

#endif
