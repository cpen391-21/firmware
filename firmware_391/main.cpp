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
#include <time.h> 
#include "rs232.h"
#include "parser.h"
#include "wifi.h"
#include "ssid.h"
#include "audio.h"

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

void test_wifi(void){
    RS232 wifi_uart(0xFF200088);
    bool timeout = false;

    char connect_cmd1[] = {'A', 'T', '+', 'C', 'W', '\0'};
    char connect_cmd[] = "AT+CWJAP=\"WiBelieveICanFi\",\"7h224x@7!s\"\r";
    char ping_cmd[] = "AT+PING=\"https://www.google.com/\"\r";

    time_t start, curr_time;
    char resp_char;
    unsigned int i;

    char resp [256];

    for(i = 0; connect_cmd[i] != '\0'; i++){
        wifi_uart.putchar(connect_cmd[i]);
    }

    time(&start);
    while (!wifi_uart.read_fifo_size()){ // wait for esp to respond
        time(&curr_time);
        if(difftime(curr_time, start) > RESP_TIMEOUT) {
            timeout = true;
            break;
        }
    }

    for(i = 0; wifi_uart.read_fifo_size() && i < 256; i++){
        wifi_uart.getchar(&resp_char);
        resp[i] = resp_char;
    }

    wifi_uart.flush();

    for(i = 0; ping_cmd[i] != '\0'; i++){
        wifi_uart.putchar(ping_cmd[i]);
    }

    time(&start);
    while (!wifi_uart.read_fifo_size()){ // wait for esp to respond
        time(&curr_time);
        if(difftime(curr_time, start) > RESP_TIMEOUT) {
            break;
        }
    }

    for(i = 0; wifi_uart.read_fifo_size() && i < 256; i++){
        wifi_uart.getchar(&resp_char);
        resp[i] = resp_char;
    }

    printf(resp);
}

void test_parser(void) {
	int len;
	char *data;
	Parser parser(&bluetooth);

	while (1) {
		len = parser.getdata(&data);
		if(len) {
			for (int i = 0; i < len; i++) {
				printf("%c", data[i]);
			}
		}
	}
}

int main(void) {
	test_wifi();
	while (1);
}
