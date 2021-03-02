#include "audio.h"
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

AudioCore::AudioCore(unsigned int address) {
	this->control_address   = (volatile unsigned int *)address;
	this->fifospace_address = (volatile unsigned int *)(address+4);
	this->ldata_address     = (volatile unsigned int *)(address+8);
	this->rdata_address     = (volatile unsigned int *)(address+12);

	// enable read interrupts
	*control_address = 0x01;
}

void AudioCore::putmono(int data) {
	*ldata_address = data;
	*rdata_address = data;
}

void AudioCore::clear_fifo(void) {
	// We must set bits 2 and 3, then reset them.
	// 0b0000_1100 is hex 0x
	*control_address = *control_address |  0x0C;
	*control_address = *control_address & ~0x0C;
}

unsigned int AudioCore::get_r_fifospace() {
	return (*fifospace_address >> 16) & 0xFF;
}

unsigned int AudioCore::get_l_fifospace() {
	return (*fifospace_address >> 24) & 0xFF;
}

unsigned int AudioCore::get_min_fifospace() {
	unsigned int leftspace  = get_l_fifospace();
	unsigned int rightspace = get_r_fifospace();

	if (leftspace < rightspace) {
		return leftspace;
	}

	return rightspace;
}
