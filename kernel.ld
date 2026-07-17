OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
	. = 1M;

	.multiboot ALIGN(8):
	{
		KEEP(*(.multiboot))
	}
	
	.text ALIGN(4K):
	{
		*(.text*)
	}

	.rodata ALIGN(4K) :
	{
		*(.rodata*)
	}

	.data ALIGN(4K) :
	{
		*(.data*)
	}

	.bss ALIGN(4K) :
	{
		*(.bss*)
		*(COMMON)
	}

	.build_note ALIGN(4) :
	{
		*(.build_note)
	}
}
