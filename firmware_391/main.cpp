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

int main(void) {

    RS232 bluetooth;
    // send and receive hello world
    bluetooth.putchar((int) 'C');
    bluetooth.putchar((int) 'P');
    bluetooth.putchar((int) 'E');
    bluetooth.putchar((int) 'N');

    // print PEN
    while(1) {
        bluetooth.getchar();
    }

	return 0;
}
