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

RS232 bluetooth(0xFF200080);

__irq void IRQHandler (void)
{
    volatile unsigned int *base = (unsigned int *) 0x80000000;

    unsigned char c;
    bluetooth.getchar(&c);
    printf("IRQ Char: %02x", c);
    //if (*base == 3)          // which interrupt was it?
    //{
    //    bluetooth.getchar();     // process the interrupt
    //}
    *(base+1) = 0;           // clear the interrupt
}


int main(void) {
    RS232 wifi_uart(0xFF200088);

    char connect_cmd[] = "AT+CWJAP=\"WiBelieveICanFi\",\"7h224x@7!s\"\r";
    char ping_cmd[] = "AT+PING=\"https://www.google.com/\"\r";

    time_t start, curr_time;
    unsigned char resp_char;
    unsigned int i;

    unsigned char resp [256];

    for(i = 0; connect_cmd[i] != '\0'; i++){
        wifi_uart.putchar(connect_cmd[i]);
    }

    time(&start);
    while (!wifi_uart.received_data()){ // wait for esp to respond
        time(&curr_time);
        if(difftime(curr_time, start) > RESP_TIMEOUT) {
            break;
        }
    }

    for(i = 0; wifi_uart.received_data() && i < 256; i++){
        wifi_uart.getchar(&resp_char);
        resp[i] = resp_char;
    }

    wifi_uart.flush();

    for(i = 0; ping_cmd[i] != '\0'; i++){
        wifi_uart.putchar(ping_cmd[i]);
    }

    time(&start);
    while (!wifi_uart.received_data()){ // wait for esp to respond
        time(&curr_time);
        if(difftime(curr_time, start) > RESP_TIMEOUT) {
            break;
        }
    }

    for(i = 0; wifi_uart.received_data() && i < 256; i++){
        wifi_uart.getchar(&resp_char);
        resp[i] = resp_char;
    }


    /*
    wifi internet(0xFF200088);
    internet.connect(WIFI_SSID, WIFI_PASSWORD);
    double RTT = internet.ping("https://www.google.com/");
    while(1);
    */

    /*
    // send and receive hello world
    bluetooth.putchar((int) 'C');
    bluetooth.putchar((int) 'P');
    bluetooth.putchar((int) 'E');
    bluetooth.putchar((int) 'N');

    char *data;
    Parser parser(&bluetooth);
    // print PEN
    while(1) {
        parser.getdata(*&data);
    }
    */
	return 0;
}
