/*
 * sdram.cpp
 *
 *  Created on: Mar 22, 2021
 *      Author: Tyler
 */

#include "sdram.h"

SDRAM::SDRAM(unsigned int address) {
	this->address = (volatile short *)address;
}

short SDRAM::get(unsigned int offset) {
	return *(address + offset);
}

void SDRAM::put(unsigned int offset, short value) {
	*(address + offset) = value;
}
