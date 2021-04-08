#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdio.h>
#include <stdlib.h>
//#include "rs232.h"

void disable_A9_interrupts(void);
void set_A9_IRQ_stack(void);
void config_GIC(void);
void enable_A9_interrupts(void);
void RS232_ISR(void);
void config_interrupt(int, int);

//extern RS232 bluetooth;

#endif /* INTERRUPT_H */
