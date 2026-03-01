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
_start:
	mov $stack_top, %esp
	cli
	call kmain
hang:
	hlt
	jmp hang

.section .build_note, "a", @note
.align 4
.long 8
.long 15
.long 70
.asciz "made by"
.align 4
.asciz "orca.pet3910YT"
.align 4
