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

.section .bss

.align 4096
.globl page_directory
page_directory:
.skip 4096
.align 4096
.globl page_table_0
page_table_0:
.skip 4096

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
	mov %eax, %esi # preserve the magic
	mov %ebx, %edx # same for the MB ptr
	mov %cr4, %eax
	orl $0x600, %eax
	mov %eax, %cr4
	mov %cr0, %eax
	andl $0xFFFFFFEB, %eax
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
	call gdt_init # initialize GDT for paging to work
	mov $page_directory, %edi
	mov $1024, %ecx
	xorl %eax, %eax
	rep stosl
	mov $page_table_0, %edi
	mov $1024, %ecx
	xorl %eax, %eax
	rep stosl
	mov $page_table_0, %eax
	xor %ebx, %ebx
	mov $1024, %ecx
	mov $page_table_0, %edi
	# no need to explain what this part does
.fill_page_table:
	mov %ebx, %eax
	orl $3, %eax
	mov %eax, (%edi)
	add $4096, %ebx
	add $4, %edi
	loop .fill_page_table
	mov $page_table_0, %eax
	orl $3, %eax
	mov %eax, (page_directory)
	# load the page directory
	mov $page_directory, %eax
	mov %eax, %cr3
	mov %cr0, %eax
	orl $0x80000001, %eax
	mov %eax, %cr0
	ljmp $0x08, $.cont
.cont:
	# push the arguments
	push %edx # mbi
	push %esi # magic
	xorl %edx, %edx
	xorl %esi, %esi
	call kmain
	push $stat_kmain_return
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
