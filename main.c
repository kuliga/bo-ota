/*
 *Bootloader's main file
 */
 
#include <stdint.h>
#include "bootloader_mem_map.h"
#include "system_MKL46Z4.h"

/*
 *Reset handler for bootloader, invoked during MCU's startup.
 *Sets clocks, initializes .bss section with zeros, copies initialized 
 *variables to .data section, branches to main().
 */
void Reset_Handler(void);

int main(void)
{
        
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
        
        main();
        
        while (1);
}
        
        
        
                                        

