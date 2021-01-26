#include <stdint.h>
#include "bootloader_mem_map.h"
#include "MKL46Z4.h"

static volatile uint8_t ab_state, ab_prev;
static const uint16_t enc_state = 0b0110100110010110;

enum baudrate {
         __115200__ = 23,
         __28800__ = 91
};

void led_init(void);

void led_deinit(void);

void gpio_init(void);

void uart0_init(enum baudrate br);

/*
 *Highly experimental handler, not necessarily needed for testing the bootloader.
 */
void PORTC_PORTD_IRQHandler(void)
{
        uint32_t gpio_state = PTC->PDIR;
        gpio_state = gpio_state >> 1;
        gpio_state &= 0b11;
        
        ab_state <<= 2;
        if (gpio_state & 0x08)
                ab_state |= 0x02;
        if (gpio_state & 0x04)
                ab_state |= 0x01;
 
        ab_state &= 0x0F;
        if (enc_state & (1 << ab_state)) {
                ab_prev <<= 4;
                ab_prev |= ab_state;
 
                switch (ab_prev & 0xFF) {
                case 0x2B:
                        UART0->D = 'R';
                        break;
                case 0x17:
                        UART0->D = 'L';
                        break;
                default:
                        break;
                }
        }
        PORTC->PCR[1] ^= PORT_ISFR_ISF(1);
        PORTC->PCR[2] ^= PORT_ISFR_ISF(2);
}
        

int main(void)
{
        led_init();
        gpio_init();
        uart0_init(__115200__);
        
        while (1);
}

/*
 *Reset handler, invoked during going to userspace.
 *Sets clocks, disables IRQs, initializes .bss section with zeros,
 *copies initialized variables to .data section,
 *branches to main().
 */
void Reset_Handler(void)
{
        SystemInit();
        
        SIM->FCFG1 |= SIM_FCFG1_FLASHDIS_MASK;
        __disable_irq();

        uint32_t *tmp = &_sbss;
        for (tmp; tmp < &_ebss; tmp++)
                *tmp = 0;
                
        tmp = &_sdata;
        uint32_t *init_vars = &_etext;
        for (tmp; tmp < &_edata; tmp++)
                *tmp = *init_vars++;
        
        tmp = (uint32_t*) &_stext;
        SCB->VTOR = ((uint32_t) tmp & SCB_VTOR_TBLOFF_Msk);
        
        
        __enable_irq();
        
        main();
        
        while (1);
}       

void led_init(void)
{
        SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
        PORTD->PCR[5] = PORT_PCR_MUX(1);
        PTD->PDDR |= 1 << 5;
}      

void led_deinit(void)
{
        PORTD->PCR[5] = PORT_PCR_MUX(0);
        SIM->SCGC5 &= ~SIM_SCGC5_PORTD_MASK;
}

void uart0_init(enum baudrate br)
{
        SIM->SOPT2 |= 1 << SIM_SOPT2_UART0SRC_SHIFT;
        SIM->SCGC4 |= 1 << SIM_SCGC4_UART0_SHIFT;;
        SIM->SCGC5 |= 1 << SIM_SCGC5_PORTA_SHIFT;
        PORTA->PCR[1] = PORT_PCR_MUX(2);
        PORTA->PCR[2] = PORT_PCR_MUX(2);
        UART0->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK );	
        UART0->BDH = 0;
        UART0->BDL = br;
        UART0->C4 &= ~UART0_C4_OSR_MASK;
        UART0->C4 |= UART0_C4_OSR(15);	
        UART0->C5 |= 1 << UART0_C5_BOTHEDGE_SHIFT;
        UART0->C2 |= UART0_C2_RE_MASK | UART0_C2_TE_MASK;
}

void gpio_init(void)
{
        SIM->SCGC5 |=  SIM_SCGC5_PORTC_MASK;
        PORTC->PCR[1] = PORT_PCR_MUX(1);
        PORTC->PCR[2] = PORT_PCR_MUX(1);
        PORTC->PCR[1] |= PORT_PCR_IRQC(10);	
        PORTC->PCR[2] |= PORT_PCR_IRQC(10);	
	NVIC_ClearPendingIRQ(PORTC_PORTD_IRQn);
        NVIC_EnableIRQ(PORTC_PORTD_IRQn);
}

        
       


                
        
        
