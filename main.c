/*
 *Bootloader's main file
 */
 
#include <stdint.h>
#include "bootloader_mem_map.h"
#include "MKL46Z4.h"

/*
 *Baud rate divisors for UART0. 
 *Be aware that these values are for this particular configuration
 *and are not inter-application-compatible.
 */
enum baudrate {
         __115200__ = 23,
         __28800__ = 91
};
 
/*
 *Initialize UART0.
 */
void uart0_poll_init(enum baudrate br);

/*
 *Deinitialize UARTO.
 *Needs to be done before going to userspace.
 */
void uart0_deinit(void);

/*
 *Initialize green led. 
 */
void led_init(void);

/*
 *Deinitialize green led.
 */
void led_deinit(void);

/*
 *Branch to userspace.
 *Loads stack pointer to msp register, then branches to the address
 *of userspace's Reset_Handler subroutine.
 *Order of passed arguments as values for MCU's registers was determined
 *during debugging process (ARM specific).
 */
__attribute__((naked)) void goto_userspace(uint32_t sp, uint32_t pc)
{
        __asm inline(
              "msr msp, r0\n\t"
              "bx r1"
              );
}


int main(void)
{      
        uart0_poll_init(__115200__);
        led_init();

        uint8_t *userspace = (uint8_t*) &__ram_userspace_start__;
        while (1) {        
                while (UART0->S1 & UART0_S1_RDRF_MASK) 
                        *userspace++ = UART0->D;                       
  
                 if (UART0->S1 & UART0_S1_IDLE_MASK) {
                        UART0->S1 = 1 << UART0_S1_IDLE_SHIFT;
                        goto exit;
                } 
        }
            
exit:  ;
       led_deinit();
       uart0_deinit();
       
       uint32_t *reset_fetch = (uint32_t*) &__ram_userspace_start__;
       uint32_t sp = *reset_fetch;
       uint32_t pc = *(reset_fetch + 1);
 
       goto_userspace(sp, pc);
       
       while (1);
}

/*
 *Reset handler, invoked during MCU's startup.
 *Sets clocks, disables IRQs, initializes .bss section with zeros,
 *copies initialized variables to .data section, fills userspace with zeros,
 *relocates .vectors table (only in user's app as for now), branches to main().
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

void uart0_poll_init(enum baudrate br)
{
        SIM->SOPT2 |= 1 << SIM_SOPT2_UART0SRC_SHIFT;
        SIM->SCGC4 |= 1 << SIM_SCGC4_UART0_SHIFT;;
        SIM->SCGC5 |= 1 << SIM_SCGC5_PORTA_SHIFT;
        PORTA->PCR[1] = PORT_PCR_MUX(2);
        PORTA->PCR[2] = PORT_PCR_MUX(2);
        UART0->BDH = 0;
        UART0->BDL = br;
        UART0->C1 |= UART0_C1_ILT_MASK;
        UART0->C4 &= ~UART0_C4_OSR_MASK;
        UART0->C4 |= UART0_C4_OSR(15);	
        UART0->C5 |= 1 << UART0_C5_BOTHEDGE_SHIFT;	
        UART0->C2 |= UART0_C2_RE_MASK | UART0_C2_TE_MASK;
}

void uart0_deinit(void)
{
        SIM->SCGC5 &= ~SIM_SCGC5_PORTA_MASK;
        SIM->SCGC4 &= ~SIM_SCGC4_UART0_MASK;
        SIM->SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
}

void led_init(void)
{    
        SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
        PORTE->PCR[29] |= PORT_PCR_MUX(1);
        PTE->PDDR |= 1 << 29;
}

void led_deinit(void)
{
        PORTE->PCR[29] = PORT_PCR_MUX(0);
        SIM->SCGC5 &= ~SIM_SCGC5_PORTE_MASK;
}       

