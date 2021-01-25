INCLUDE mem.ld

SECTIONS 
{
    .text : 
    {
         . = ALIGN(4);
        _stext = .;
        KEEP(*(.vectors .vectors.*))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
       _etext = .;
    } > RAM_USERSPACE

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
         _ebss = .;
    } > RAM

    /*
     *Look at this like RAM is VMA, whereas RAM_USERSPACE is LMA
     */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > RAM_USERSPACE

    .stack (NOLOAD) :
    {
        . = ALIGN(8);
        _sstack = .;
        . = . + __stack_size__;
        . = ALIGN(8);
        _estack = .;
    } > RAM

    _end = .;
}