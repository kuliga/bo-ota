/*
 *Bootloader's main file
 */
 
#include <stdint.h>
#include "bootloader_mem_map.h"
#include "MKL46Z4.h"
#include "kl46z_startup.h"
 
/*
 *Initialize UART0
 */
void uart0_poll_init(void);

/*
 *TODO: add comments
 */
__attribute__((naked)) void goto_userspace(uint32_t pc, uint32_t sp)
{
         __asm inline(
              "msr msp, r1\n\t"
              "bx r0"
              );
}


int main(void)
{
        uart0_poll_init();

        uint8_t *userspace = (uint8_t*) &__ram_userspace_start__;
        while (1) {        
                while (UART0->S1 & UART0_S1_RDRF_MASK) 
                        *userspace++ = UART0->D;
                
                if (UART0->S1 & UART0_S1_IDLE_MASK) {
                        UART0->S1 = 1 << UART0_S1_IDLE_SHIFT;
                        goto exit;
                }
        }
        
exit: ;
       uint32_t *reset_fetch = (uint32_t*) &__ram_userspace_start__;
       uint32_t sp = *reset_fetch;
       uint32_t pc = *(reset_fetch + 1);
       
       goto_userspace(pc, sp);
       
       while (1);
}

void uart0_poll_init(void)
{
        SIM->SOPT2 |= 1 << SIM_SOPT2_UART0SRC_SHIFT;
        SIM->SCGC4 |= 1 << SIM_SCGC4_UART0_SHIFT;
        SIM->SCGC5 |= 1 << SIM_SCGC5_PORTA_SHIFT;
        PORTA->PCR[1] = PORT_PCR_MUX(2);
        UART0->BDH = 0;
        UART0->BDL = 23;
        UART0->C1 |= UART0_C1_ILT_MASK;
        UART0->C4 &= ~UART0_C4_OSR_MASK;
        UART0->C4 |= UART0_C4_OSR(15);	
        UART0->C5 |= UART0_C5_BOTHEDGE_MASK;	
        UART0->C2 |= 1 << UART0_C2_RE_SHIFT;
}

/*
 *Reset handler, invoked during MCU's startup.
 *Sets clocks, disables IRQs, initializes .bss section with zeros,
 *copies initialized variables to .data section, fills userspace with zeros,
 *relocates .vectors table, branches to main().
 */
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
        
	/*ONLY FOR BOOTLOADER*/        
	tmp = &__ram_userspace_start__;
        for (tmp; tmp < &__ram_start__; tmp++)
                *tmp = 0;
                
        main();
        
        while (1);
}       
        
                                        

