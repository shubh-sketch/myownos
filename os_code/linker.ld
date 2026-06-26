ENTRY(_start)
SECTIONS
{
    . = 0x100000; /* Start layout at 1MB physical RAM space */

    .text : {
        *(.multiboot)
        *(.text)
    }

    .rodata : { *(.rodata) }
    .data   : { *(.data) }
    .bss    : { *(.bss) }
}