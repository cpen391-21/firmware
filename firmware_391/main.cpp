/*
 ============================================================================
 Name        : uart_tutorial.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "rs232.h"
#include "parser.h"

RS232 bluetooth(0xFF200080);

__irq void IRQHandler (void)
{
    volatile unsigned int *base = (unsigned int *) 0x80000000;

    unsigned char c;
    bluetooth.getchar(&c);
    printf("IRQ Char: %02x", c);
    //if (*base == 3)          // which interrupt was it?
    //{
    //    bluetooth.getchar();     // process the interrupt
    //}
    *(base+1) = 0;           // clear the interrupt
}


int main(void) {
    // send and receive hello world
    bluetooth.putchar((int) 'C');
    bluetooth.putchar((int) 'P');
    bluetooth.putchar((int) 'E');
    bluetooth.putchar((int) 'N');

    char *data;
    Parser parser(&bluetooth);
    // print PEN
    while(1) {
        parser.getdata(*&data);
    }

	return 0;
}
