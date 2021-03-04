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

/* Big enough for 1 second of audio */
#define AUDIO_DATA_MAX_SIZE 48000

RS232 bluetooth(0xFF200080);
AudioCore audio(0xFF200090);

char outbuffer[256];

short audio_data[AUDIO_DATA_MAX_SIZE];
unsigned int audio_data_size;
unsigned int audio_data_position;
bool enable_audio = true;

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

		if (fifospace) {
			audio.putmono(data);

			if (count++ >= 31) {
				count = 0;
				if (data == 0) {
					data = 0xFFFF;
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

void test_parser(void) {
	int len;
	char *data;
	Parser parser(&bluetooth);

	while (1) {
		len = parser.getdata(&data);
		/*if(len) {
			for (int i = 0; i < len; i++) {
				printf("%c", data[i]);
			}
		}*/
	}
}

int compare_string_start(const char *s1, const char *s2) {
	for (; *s1 == *s2; s1++, s2++) {};

	// these arrays have been the same up until a null character
	if (*s1 == '\0' || *s2 == '\0') {
		return 0;
	}
	return -1;
}

void send_mono_audio() {
	if (audio.get_min_fifospace() == 0) {
		return;
	}

    while (audio.get_min_fifospace()) {
        audio.putmono(audio_data[audio_data_position]);
        audio_data_position = (audio_data_position + 1) % audio_data_size;
    }
}

void update_audio_data(char *datastring) {
	// Datastring is as follows:
	// 'd:' (16 bits) | 16 bit data (big endian) | address (24 bit, big endian)
	short data = (*(datastring+2) << 8) + *(datastring+3);
	unsigned int address = (*(datastring+4) << 16) +
                           (*(datastring+5) << 8 ) +
                           (*(datastring+6)      );

	sprintf(outbuffer, "Data: %2d, Address, %d (%02x, %02x, %02x)\n",
			data, address,
			*(datastring+4),
			*(datastring+5),
			*(datastring+6)
			);
	bluetooth.sendmsg(outbuffer);

	if (address > AUDIO_DATA_MAX_SIZE) {
		printf("Address out of bounds!\n");
		return;
	}

    audio_data[address] = data;
}

void update_size(char *datastring) {
	// Datastring is as follows:
	// 's:' (16 bits) | size (24 bit, big endian)
	unsigned int size = (*(datastring+2) << 16) +
                        (*(datastring+3) << 8 ) +
                        (*(datastring+4)      );

	sprintf(outbuffer, "New audio data size: %d\n\0", size);
	bluetooth.sendmsg(outbuffer);

    audio_data_size = size;
}

void print_byte_array(char *bytearr, int len) {
	for (int i = 0; i < len; i++) {
		printf("%02x ", bytearr[i]);
	}
	printf("\n");
}

void mono_bt_player(void) {
	int len;
	char *data;
	char *data_keyword  = "d:\0";
	char *vol_keyword   = "v:\0"; // out of 16, we just do a bit shift.
    char *start_keyword = "start\0";
    char *stop_keyword  = "stop\0";
    char *size_keyword  = "s:\0";
	int uart_fifo;

	Parser parser(&bluetooth);

	while (1) {
		uart_fifo = bluetooth.read_fifo_size();
		if (uart_fifo > 127) {
			bluetooth.sendmsg("Warning: UART fifo is full!\n\0");
		}

        // parser will not return until a message is delivered.
        // It's important to check if there is a message or it will interrupt our data.
		len = parser.getdata(&data);

		if(len) {
			if(!compare_string_start(data, data_keyword)) {
				update_audio_data(data);
			} else if (!compare_string_start(data, start_keyword)) {
                enable_audio = true;
                bluetooth.sendmsg("Audio output started\n\0");
            } else if (!compare_string_start(data, stop_keyword)) {
                enable_audio = false;
                bluetooth.sendmsg("Audio output stopped\n\0");
            } else if (!compare_string_start(data, size_keyword)) {
                update_size(data);
            }
		}

        if (enable_audio) {
            send_mono_audio();
        }
	}
}

int main(void) {
	mono_bt_player();
}
