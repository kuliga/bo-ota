/*
 *Bootloader's main file
 */
 
#include <stdint.h>
#include "bootloader_mem_map.h"
#include "system_MKL46Z4.h"
#include "MKL46Z4.h"

/*
 *Reset handler for bootloader, invoked during MCU's startup.
 *Sets clocks, initializes .bss section with zeros, copies initialized 
 *variables to .data section, branches to main().
 */
void Reset_Handler(void);

/*
 *TODO: add comments
 */
 /*
__attribute__((naked)) inline void goto_userspace(void)
{
        __asm inline (
              "ldr r0, =%[sp]\n\t"
              "ldr r1, =%[_start_userspace_]\n\t"
              "msr msp, r0\n\t"
              "bx r1"
              : :[sp] "r" (sp), [start_userspace] "r" (START_USERSPACE)
             );
}
*/
int main(void)
{
        uint32_t *userspace = (uint32_t*) __rom_userspace_start__;
        uint32_t x = *userspace;
}


void Reset_Handler(void)
{
        SystemInit();
        
        uint32_t *bss = &_sbss;
        for (bss; bss < &_ebss; bss++)
                *bss = 0;
                
        uint32_t *data = &_sdata;
        uint32_t *init_vars = &_etext;
        for (data; data < &_edata; data++)
                *data = *init_vars++;
        //SIM->FCFG1 = 1;
        
        main();
        
        while (1);
}
        
        
        
                                        

