ENTRY (_start)

SECTIONS
{
    . = 0;
    interrupt_vector_table = 0;
    _kstart = .;
    .text : { *(.text*) *(.rodata*) }
    .data : { *(.data*) }
    .bss : { *(.bss*) *(COMMON*) }
    _kend = .;
}

