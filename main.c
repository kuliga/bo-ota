/*
 *Bootloader's main file
 */
 
#include <stdint.h>
#include "bootloader_mem_map.h"
#include "system_MKL46Z4.h"
#include "MKL46Z4.h"

/*
 *Vector Table Offset Register
 */
#define VTOR_REG 0xE000ED08
#define VTOR_TBLOFF_MASK                (0xFFFFFFUL << 8)    

/*
 *Reset handler, invoked during MCU's startup.
 *Sets clocks, disables IRQs, initializes .bss section with zeros,
 *copies initialized variables to .data section, fills userspace with zeros,
 *relocates .vectors table, branches to main().
 */
void Reset_Handler(void);

/*
 *Initialize UART0
 */
void uart0_poll_init(void);

/*
 *TODO: add comments
 */

__attribute__((naked)) void goto_userspace(void)
{
        __asm inline (
              "ldr r0, [%[sp]]\n\t"
              "ldr r1, [%[pc]]\n\t"
              "msr msp, r0\n\t"
              "bx r1"
              : :[sp] "r" (__ram_userspace_start__), [pc] "r" (__ram_userspace_start__ + 1)
             );
}


int main(void)
{
        uart0_poll_init();

        uint32_t *userspace = (uint32_t*) &__ram_userspace_start__;
        while (1) {        
                while (UART0->S1 & UART0_S1_RDRF_MASK) {
                       *userspace++ = UART0->D;
                        if (UART0->S1 & UART0_S1_IDLE_MASK) {
                                UART0->S1 |= 1 << UART0_S1_IDLE_SHIFT;
                                goto exit;
                        }
                }
        }
exit:           
        //SIM->FCFG1 |= SIM_FCFG1_FLASHDIS_MASK;
        goto_userspace();
               
        while (1);
}


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
        
        
        
                                        

