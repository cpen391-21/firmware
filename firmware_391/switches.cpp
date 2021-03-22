/*
 * switches.cpp
 *
 *  Created on: Mar 22, 2021
 *      Author: Tyler
 */

#include "switches.h"

Switches::Switches(unsigned int address) {
	this->address = (volatile unsigned int *)address;
	this->oldval = 0;
}

bool Switches::newval() {
	unsigned int tempval = oldval;
	oldval = *address;

	return (oldval != tempval);
}

unsigned int Switches::get() {
	return oldval;
}
