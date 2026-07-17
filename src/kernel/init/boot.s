.section .multiboot, "a"
.align 8
multiboot_header:
# magic and start of tags
.align 8
.long 0xE85250D6
.long 0
.long multiboot_head_end - multiboot_header
.long -(0xE85250D6+0+(multiboot_head_end-multiboot_header))

# framebuffer request
.align 8
.short 5
.short 0
.long 20
.long 1920
.long 1080
.long 32

# end tag
.align 8
.short 0
.short 0
.long 8
multiboot_head_end:

#.align 4
#.long 0x1BADB002
#.long (1 << 2) | (1 << 1)
#.long -(0x1BADB002 + ((1 << 2) | 1 << 1)) & 0xFFFFFFFF
#.skip 24
#.long 0
#.long 1920
#.long 1080
#.long 32


#.long 1024
#.long 768
#.long 32

.section .bss
.globl cpu_vendor
cpu_vendor:
.skip 13
.align 16
stack_bottom:
.skip 16384
.align 16
stack_top:

.section .text
.global _start
.type _start, @function
.extern kmain
.extern panic
_start:
	xor %ebp, %ebp # zero out ebp
	# enable SSE
	mov %ecx, %eax # preserve the magic
	mov %cr4, %eax
	orl $0x600, %eax
	mov %eax, %cr4
	mov %cr0, %eax
	andl $0xFFFFFFFB, %eax
	orl $0x0002, %eax
	mov %eax, %cr0
	# set up stack
	mov $stack_top, %esp
	cli
	cld
	nopw %cs:0x0(%eax,%eax,1)
	# initialize the FPU
	fwait
	fninit
	push $stat_boot_fpu_init
	call printk
	push $stat_boot_init
	call printk
	pop %ecx
	# push the arguments
	push %ebx # mbi
	push %ecx # magic
	call kmain
	push stat_kmain_return
	call panic

.section .rodata
stat_kmain_return:
.asciz "BAD C: `kmain` returned\0"
stat_boot_init:
.asciz "Boot stub: kernel initialized\0"
stat_boot_fpu_init:
.asciz "Boot stub: x87 FPU initialized\0"
.section .build_note, "a", @note
.align 4
.long 8
.long 15
.long 2
.asciz "made by"
.align 4
.asciz "orca.pet3910YT"
.align 4
