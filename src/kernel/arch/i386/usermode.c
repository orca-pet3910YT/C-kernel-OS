void test_usermode() {
	__asm__ volatile ("hlt"); // this should ideally raise an interrupt;
				// hlt is an illegal instruction in usermode
}
