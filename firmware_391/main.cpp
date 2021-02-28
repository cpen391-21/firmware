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

/* 8 byte wide (0x0080-0x0087).
 * Find Atlera guide "RS232 UART for Altera DE-Series Boards"
 * For programming model and registers
 */
#define UART_data    0xFF200080;
//#define UART_control (*(volatile unsigned int *)0xFF200084);

//volatile unsigned int *UART_data = 0xFF200080;

/*
 * ARM is little-endian so the lsb of the 32-bit word is at 0x80.
 * When we load the data in address UART_data, we load 4 memory addresses
 * (each 8 bits wide) into a 32-bit wide register
 */

int putcharRS232(char c)
{
	// C should only be one byte
	c = c & 0xFF;

	printf("Putting char %c\n", c);

	/* write character to Transmitter fifo register */
	*(volatile unsigned int *)0xFF200080 = c;

	return c;
}

int getcharRS232()
{

	unsigned int data = *(volatile unsigned int *)0xFF200080;
	unsigned int data_valid = data & 0x8000;
	unsigned int chars_left = (data >> 16) & 0xFF;

	// byte 15 of data is the "RVALID" flag. Check this.
	if (data_valid && chars_left) {
		printf("%d characters remaining in the read FIFO ", chars_left);
		printf("Data field is valid, ");
		printf("returning char %c\n", (data & 0xFF));

		// I'm pretty sure I'm doing parity wrong
		if (!(data & 0200)){
			printf("But there is a parity error. Disregard\n");
		}
		return data & 0xFF;
	}

	// return only the last byte of the word
	return 0;
}

// the following function polls the UART to determine if any character
// has been received. It doesn't wait for one, or read it, it simply tests
// to see if one is available to read from the FIFO
int RS232TestForReceivedData(void)
{
	int data = *(volatile unsigned int *)0xFF200080;
	return (data >> 16) & 0xFF;
}

//
// Remove/flush the UART receiver buffer by removing any unread characters
//
void RS232Flush(void)
{
	while (RS232TestForReceivedData()) {
		getcharRS232();
	};
	return; // no more characters so return
}

int main(void) {

    // send and receive hello world
    putcharRS232((int) 'C');
    //RS232Flush(); // get rid of C
    putcharRS232((int) 'P');
    putcharRS232((int) 'E');
    putcharRS232((int) 'N');

    // print PEN
    while(1) {
        getcharRS232();
    }

	return 0;
}