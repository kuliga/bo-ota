#ifndef KL46Z_STARTUP_H
#define KL46Z_STARTUP_H

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

#endif