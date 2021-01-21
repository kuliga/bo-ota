#include <stdint.h>
#include "kl46z_startup.h"
#include "system_MKL46Z4.h"
#include "MKL46Z4.h"
#include "bootloader_mem_map.h"

void Reset_Handler(void)
{
        SystemInit();
        
        __disable_irq();
        
        uint32_t *tmp = &_sbss;
        for (tmp; tmp < &_ebss; tmp++)
                *tmp = 0;
                
        tmp = &_sdata;
        uint32_t *init_vars = &_etext;
        for (tmp; tmp < &_edata; tmp++)
                *tmp = *init_vars++;
        
        tmp = &__ram_userspace_start__;
        for (tmp; tmp < &__ram_start__; tmp++)
                *tmp = 0; 
        
        tmp = &_stext;
        uint32_t *vtor = (uint32_t*) VTOR_REG;
        *vtor = ((uint32_t) tmp & VTOR_TBLOFF_MASK);
                
        main();
        
        while (1);
}
