/*
 *Bootloader's main file
 */
 
#include <stdint.h>
#include "bootloader_mem_map.h"
#include "system_MKL46Z4.h"
#include "MKL46Z4.h"

/*
 *Reset handler for bootloader, invoked during MCU's startup.
 *Sets clocks, disables IRQs, initializes .bss section with zeros,
 *copies initialized variables to .data section, fills userspace with zeros,
 *branches to main().
 */
void Reset_Handler(void);

/*
 *Initialize UART0
 */
void uart0_poll_init(void);

/*
 *TODO: add comments
 */
/*
__attribute__((naked)) inline void goto_userspace(void)
{
        __asm inline (
              "ldr r0, =%[_sp]\n\t"
              "ldr r1, =%[_pc]\n\t"
              "msr msp, r0\n\t"
              "bx r1"
              : :[_sp] "r" (sp), [_pc] "r" (pc)
             );
}*/

int main(void)
{
        uart0_poll_init();
        uint32_t *userspace = (uint32_t*) __ram_userspace_start__;
        uint32_t x = *userspace;
        
        while (1);
}


void Reset_Handler(void)
{
        SystemInit();
        
        __disable_irq();
        
        uint32_t *bss = &_sbss;
        for (bss; bss < &_ebss; bss++)
                *bss = 0;
                
        uint32_t *data = &_sdata;
        uint32_t *init_vars = &_etext;
        for (data; data < &_edata; data++)
                *data = *init_vars++;
        
        uint32_t *usrspc = &__ram_userspace_start__;
        for (usrspc; usrspc < &__ram_start__; usrspc++)
                *usrspc = 0; 
        
        main();
        
        while (1);
}

void uart0_poll_init(void)
{
        SIM->SOPT2 |= 1 << SIM_SOPT2_UART0SRC_SHIFT;
        SIM->SCGC4 |= 1 << SIM_SCGC4_UART0_SHIFT;
        SIM->SCGC5 |= 1 << SIM_SCGC5_PORTA_SHIFT;
        PORTA->PCR[1] = PORT_PCR_MUX(2);
        UART0->BDH = 0;
        UART0->BDL = 91;
        UART0->C4 &= ~UART0_C4_OSR_MASK;
        UART0->C4 |= UART0_C4_OSR(15);	
        UART0->C5 |= UART0_C5_BOTHEDGE_MASK;	
        UART0->C2 |= 1 << UART0_C2_RE_SHIFT;
}
        
        
        
                                        

