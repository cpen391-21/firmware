/*
 * sdram.h
 *
 *  Created on: Mar 22, 2021
 *      Author: Tyler
 */

#ifndef SDRAM_H_
#define SDRAM_H_

#include <stdio.h>

class SDRAM {
private:
	volatile short *address;
public:
	SDRAM(unsigned int address);
	short get(unsigned int offset);
	void put(unsigned int offset, short value);
};


#endif /* SDRAM_H_ */
