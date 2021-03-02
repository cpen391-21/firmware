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
#include "audio.h"
#include "rs232.h"
#include "parser.h"


RS232 bluetooth(0xFF200080);
AudioCore audio(0xFF200090);

void test_audio(void) {
	// we need to send 16 bit data to the device
	short data = 0;
	int count = 0;
	int fifospace;

	// first make sure we get something.
	printf("Fifo space left: %d, %d\n",
			audio.get_l_fifospace(),
			audio.get_r_fifospace());

	while(1) {
		fifospace = audio.get_min_fifospace();

		printf("Fifospace: %d, data %d\n", fifospace, data);

		if (fifospace) {
			audio.putmono(data);

			if (count++ >= 3) {
				count = 0;
				if (data == 0) {
					data = 0xFF;
				} else {
					data = 0;
				}
			}
		}
	}
}

void test_bluetooth(void){
    char str [] = "From the ARM A9! Over Bluetooth!";
    for (int i = 0; str[i] != '\0'; i++) {
        bluetooth.putchar(str[i]);
    }

    char c;
    int result;

    while(1) {
    	result = bluetooth.getchar(&c);
    	if (!result) {
    		printf("Looping back char %02x\n", c);
    		bluetooth.putchar(c);
    	}
    }
}

int main(void) {
	test_bluetooth();
}
