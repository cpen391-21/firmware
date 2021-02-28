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

RS232::RS232(unsigned int address) {
	this->address = address;
	this->control_address = address+4;

	// enable read interrupts
	*(volatile unsigned int *)control_address = 0x01;
}

int RS232::putchar(char c)
{
	// C should only be one byte
	c = c & 0xFF;

	printf("Putting char %c\n", c);

	/* write character to Transmitter fifo register */
	*(volatile unsigned int *)address = c;

	return c;
}

int RS232::getchar(unsigned char *c)
{

	unsigned int data = *(volatile unsigned int *)address;
	unsigned int data_valid = data & 0x8000;
	unsigned int chars_left = (data >> 16) & 0xFF;

	unsigned int control = *(volatile unsigned int *)control_address;

	*c = data & 0xFF;
	// byte 15 of data is the "RVALID" flag. Check this.
	if (data_valid) {
		//printf("%d characters remaining in the read FIFO ", chars_left);
		//printf("Data field is valid, ");
		//printf("returning char %02x\n", *c);

		// Bluetooth chip has no parity so we can ignore that.
		// We will CRC32 our data instead.
		return 0;
	}

	// return only the last byte of the word
	return -1;
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
