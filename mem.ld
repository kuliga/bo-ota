MEMORY
{
  ROM_BOOTLOADER (rx) : ORIGIN = 0x00000000, LENGTH = 0x00004000
  RAM_SHARED (rwx) : ORIGIN = 0x1FFFE000, LENGTH = 0x00000800
  RAM_USERSPACE (rwx) : ORIGIN = 0x1FFFE800, LENGTH = 0x00003800
  RAM (rwx) : ORIGIN = 0x20002000, LENGTH = 0x00004000
}

__stack_size__ = 0x0D00;
__rom_bootloader_start__ = ORIGIN(ROM_BOOTLOADER);
__ram_userspace_start__ = ORIGIN(RAM_USERSPACE);
__ram_shared_start__ = ORIGIN(RAM_SHARED);
__ram_userspace_size__ = LENGTH(RAM_USERSPACE);
__ram_start__ = ORIGIN(RAM);