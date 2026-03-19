.section .multiboot
.align 4
.long 0x1BADB002
.long 0x00000000
.long -(0x1BADB002)

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.global _start
.type _start, @function
.extern kmain
.extern panic
_start:
	mov $stack_top, %esp
	cli
	cld
	push $stat_boot_init
	call printk
	pop %ecx
	push %ebx
	push %eax
	call kmain
	push $stat_kmain_return
	call panic
hang:
	hlt
	jmp hang

.section .rodata
stat_kmain_return:
.asciz "BAD C: `kmain` returned"
stat_boot_init:
.asciz "Boot stub: kernel initialized"

.section .build_note, "a", @note
.align 4
.long 8
.long 15
.long 70
.asciz "made by"
.align 4
.asciz "orca.pet3910YT"
.align 4
