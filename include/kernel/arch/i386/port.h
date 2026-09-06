#ifndef PORT_H
#define PORT_H
#include <stdint.h>

__attribute__((always_inline))
static inline uint8_t inb(uint16_t port) {
        uint8_t data;
        __asm__ volatile ("inb %1, %0" : "=a" (data) : "dN" (port));
        return data;
}

__attribute__((always_inline))
static inline void outb(uint16_t port, uint8_t data) {
        __asm__ volatile ("outb %0, %1" : : "a" (data), "dN" (port));
}

__attribute__((always_inline))
static inline uint16_t inw(uint16_t port) {
	uint16_t data;
	__asm__ volatile ("inw %1, %0" : "=a" (data) : "dN" (port));
	return data;
}

__attribute__((always_inline))
static inline void outw(uint16_t port, uint16_t data) {
	__asm__ volatile ("outw %0, %1" : : "a" (data), "dN" (port));
}

__attribute__((always_inline))
static inline uint32_t inl(uint16_t port) {
	uint32_t data;
	__asm__ volatile ("inl %1, %0" : "=a" (data) : "dN" (port));
	return data;
}

__attribute__((always_inline))
static inline void outl(uint16_t port, uint32_t data) {
	__asm__ volatile ("outl %0, %1" : : "a" (data), "dN" (port));
}

__attribute__((always_inline))
static inline void wait_port() {
        __asm__ volatile ("outb %%al, $0x80" : : "a"(0));
}

__attribute__((always_inline))
static inline void set_post(uint8_t value) {
        outb(0x80, value);
}

static inline void sti() {
	__asm__ volatile ("sti");
}
static inline void cli() {
	__asm__ volatile ("cli");
}
#endif
