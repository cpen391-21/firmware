/*
 * switches.h
 *
 *  Created on: Mar 22, 2021
 *      Author: Tyler
 */

#ifndef SWITCHES_H_
#define SWITCHES_H_


class Switches {
	private:
		volatile unsigned int *address;
		unsigned int oldval;
	public:
		Switches(unsigned int address);
		bool newval();
		unsigned int get();
};


#endif /* SWITCHES_H_ */
