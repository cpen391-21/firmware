#include "rs232.h"
#include <stdlib.h>
#include <stdio.h>

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

int RS232::putchar(char c)
{
	// C should only be one byte
	c = c & 0xFF;

	printf("Putting char %c\n", c);

	/* write character to Transmitter fifo register */
	*(volatile unsigned int *)0xFF200080 = c;

	return c;
}

int RS232::getchar()
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

int RS232::received_data(void)
{
	int data = *(volatile unsigned int *)0xFF200080;
	return (data >> 16) & 0xFF;
}

//
// Remove/flush the UART receiver buffer by removing any unread characters
//
void RS232::flush(void)
{
	while (received_data()) {
		flush();
	};
	return; // no more characters so return
}
