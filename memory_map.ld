MEMORY
{
  ROM_BOOTLOADER (rx) : ORIGIN = 0x00000000, LENGTH = 0x00004000
  RAM_SHARED (rwx) : ORIGIN = 0x1FFFE000, LENGTH = 0x00000800
  RAM_USERSPACE (rwx) : ORIGIN = 0x1FFFE800, LENGTH = 0x00003800
  RAM (rwx) : ORIGIN = 0x20002000, LENGTH = 0x00004000
}

STACK_SIZE = 0x0D00;
__rom_bootloader_start__ = ORIGIN(ROM_BOOTLOADER);
__ram_userspace_start__ = ORIGIN(RAM_USERSPACE);
__ram_shared_start__ = ORIGIN(RAM_SHARED);

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
    } > ROM_BOOTLOADER

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
     *Look at this like RAM is VMA, whereas ROM is LMA
     */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > ROM_BOOTLOADER

    .stack (NOLOAD) :
    {
        . = ALIGN(8);
        _sstack = .;
        . = . + STACK_SIZE;
        . = ALIGN(8);
        _estack = .;
    } > RAM

    _end = .;
}