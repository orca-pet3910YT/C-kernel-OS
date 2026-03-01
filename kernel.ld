OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    . = 1M;

    .text :
    {
	KEEP(*(.multiboot))
        *(.text*)
	*(.rodata)
	*(.data)
    }

    .bss :
    {
        *(.bss*)
        *(COMMON)
    }

    .build_note :
    {
        *(.build_note)
    }
}
