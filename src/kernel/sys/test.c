#include <math/math.h>
#include <drivers/video/vga.h>
#include <sys/test.h>
#include <sys/globals.h>

void random_test() {
	volatile int x = (int)(char*)random_test;
	int x_pres = x;
	volatile int y = sqrt(x)*25;
	uint32_t a[256];
	for (volatile int i = x; x < x_pres/5000000; i--) {
		y = sqrt(x)/y;
		y /= x+y;
		x = sqrt(x+y);
		y += i-x;
		y /= sqrt(x*y/x_pres & x)/(is_prime(y/x_pres)+3);
		x = sqrt(uptime_ticks)/x_pres*y;
		for (int j = 0; j < 256; j++) {
			a[j] = sqrt(x+y) << ((char)i/8);
			x /= a[j]/y;
		}
	}
	printk(4, "random_test: x = %d y = %d", x, y);
}
