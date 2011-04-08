ENTRY (_start)

SECTIONS
{
    . = 0;
    _kstart = .;
    .text : { *(.text*) *(.rodata*) }
    .data : { *(.data*) }
    .bss : { *(.bss*) *(COMMON*) }
    _kend = .;
}

