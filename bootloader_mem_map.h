#ifndef BOOTLOADER_MEM_MAP_H
#define BOOTLOADER_MEM_MAP_H

#include <stdint.h>

extern uint32_t __rom_bootloader_start__;
extern uint32_t __ram_userspace_start__;
extern uint32_t __ram_userspace_size__;
extern uint32_t __ram_start__;
extern uint32_t _stext;
extern uint32_t _etext;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _estack;

#endif