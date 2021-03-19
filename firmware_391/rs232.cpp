#include "rs232.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * Find Atlera guide "RS232 UART for Altera DE-Series Boards"
 * For programming model and registers
 *
 * The module is 16-byte memory mapped (for example, 0x80-0x87)
 * Data reg is 8 byte wide             (for example, 0x80-0x83)
 * Control reg is 8 bytes wide         (for example, 0x84-0x87)
 *
 * ARM is little-endian so the lsb of the 32-bit word is at 0x80.
 * When we load the data in address UART_data, we load 4 memory addresses
 * (each 8 bits wide) into a 32-bit wide register
 */

RS232::RS232(unsigned int address)
{
	this->address = (volatile unsigned int *)address;
	this->control_address = (volatile unsigned int *)(address+4);

	// enable read interrupts
	*control_address = 0x01;
}

int RS232::putchar(char c)
{
	// Redundant. But ensure char is only 1 byte.
	c = c & 0xFF;

	/* write character to Transmitter fifo register */
	*address = c;

	return c;
}

void RS232::sendmsg(char *c) {
	while (*c != '\0') {
		putchar(*c);
		c++;
	}
}

int RS232::getchar(char *c)
{
	unsigned int data = *address;
	unsigned int data_valid = data & 0x8000;

	if (data_valid) {
		*c = data & 0xFF;
		return 0;
	}

	return -1;
}

int RS232::read_fifo_size(void)
{
	int data = *(volatile unsigned int *)0xFF200080;
	return (data >> 16) & 0xFF;
}

//
// Remove/flush the UART receiver buffer by removing any unread characters
//
void RS232::flush(void)
{
	char c;
	while (read_fifo_size()) {
		getchar(&c);
	};
	return; // no more characters so return
}
