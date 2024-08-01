OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
SECTIONS
{
    . = 0x00100000;
    
    .text ALIGN(4) : { *(.text) }
    .data ALIGN(4) : { *(.data) }
    .bss  ALIGN(4) : { *(.bss) }
}