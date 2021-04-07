/*
 * tests.cpp
 *
 *  Created on: Mar 22, 2021
 *      Author: Tyler
 */

#include "tests.h"
#include "control.h"
#include "parser.h"

AudioCore               audio(0xFF200090);
Parser			 parse_tester(&bluetooth);

char outbuffer[256];

short        audio_data[AUDIO_DATA_MAX_SIZE];
unsigned int audio_data_size = 1;
unsigned int audio_data_position = 0;

bool double_cmp(double a, double b) {
	return (a - b) < 0.0001 || (a - b) > 0.0001;
}

bool single_parser_test(char *str, bt_command expected) {
	bt_command result = {NEW_WAVE, 0.0, 0.0, 0.0};
	parse_tester.reset_bt_parser();
	int cmd_id = -1;

	for (unsigned int i = 0; str[i] != '\0'; i++) {
		cmd_id = parse_tester.parse_bluetooth_char(str[i], &result);
	}

	return cmd_id == (int) expected.cmd &&
		result.cmd == expected.cmd &&
		double_cmp(result.param1, expected.param1) &&
		double_cmp(result.param2, expected.param2) &&
		double_cmp(result.param3, expected.param3);
}

void test_bt_parser(void) {
	unsigned int successes = 0;
	bt_command results[NUM_PARSER_TESTS] = {
		{NEW_WAVE, 12.34, 0.0, 0.0},
		{ADD_SINE, 8, 7.0, 0.0},
		{ADD_TRIANGLE, 0, 0, 0.0},
		{ADD_RANDOM, 5.3453, 4.2, 0.0}
	};

	char command_strs[NUM_PARSER_TESTS][32] = {
		"EN+NEW_WAVE,12.34\n",
		"EN+ADD_SINE,8.0,7\n",
		"EN+ADD_TRIANGLE,0,0\n",
		"EN+ADD_RANDOM,5.3453,4.2\n"
	};

	for (unsigned int i = 0; i < NUM_PARSER_TESTS; i++) {
		if (single_parser_test(command_strs[i], results[i])) successes ++;
	}

	sprintf(outbuffer, "Out of %d parser tests, %d passed. \n", NUM_PARSER_TESTS, successes);
}

void test_audio_square(void) {
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

void setup_audio_sine() {
	int size = 4096;
	double result;
	short data = 0;

	for (int i = 0; i < size; i++) {
		result = sin(2 * 3.14159 * i/256) + (sin(2 * 3.14159 * i/128) / 8);
		result *= amplitude;
		data   = (short)result;
		audio_data[i] = data;
	}

	audio_data_size = size;
}

void test_audio_sine() {
	int i = 0;
	short data;

	while (1) {
		if (audio.get_min_fifospace()) {
				data = audio_data[i++];
				audio.putmono(data);

				if (i == audio_data_size) {
					i = 0;
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
        audio.putmono(audio_data[audio_data_position++]);

        if (audio_data_position == audio_data_size) {
        	audio_data_position = 0;
        }
    }
}

void update_audio_data(char *datastring) {
	// Datastring is as follows:
	// 'd:' (16 bits) | 16 bit data (big endian) | address (24 bit, big endian)
	short data = (*(datastring+2) << 8) + *(datastring+3);
	unsigned int address = (*(datastring+4) << 16) +
                           (*(datastring+5) << 8 ) +
                           (*(datastring+6)      );

	sprintf(outbuffer, "Data: %06d, Address %d (%02x, %02x, %02x)\n",
			data, address,
			*(datastring+4),
			*(datastring+5),
			*(datastring+6)
			);
	bluetooth.sendmsg(outbuffer);

	if (address > AUDIO_DATA_MAX_SIZE) {
		bluetooth.sendmsg("Address out of bounds!\n");
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

	bool enable_audio = false;

	Parser parser(&bluetooth);

	char c;

	while (1) {
		uart_fifo = bluetooth.getchar(&c);
		if (uart_fifo > 127) {
			bluetooth.sendmsg("Warning: UART fifo is full!\n\0");
		}

        // parser will not return until a message is delivered.
        // It's important to check if there is a message or it will interrupt our data.
		/*
		if (parser.startflag()) {
			len = parser.getdata(&data);
		} else {
			len = 0;
		}
		*/

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

        if (enable_audio && false) {
            send_mono_audio();
        }
	}
}

void test_sdram(void) {

	// Test all 32M (2^25) memory locations. 33554432
	for (unsigned int i = 0; i < SDRAM_LEN; i++) {
		sdram.put(i, i);
		if (!(i % (SDRAM_LEN/16))) {
			printf("Put Progress: %d/%d\n", i, SDRAM_LEN);
		}
	}

	for (unsigned int i = 0; i < SDRAM_LEN; i++) {
		short d = sdram.get(i);
		if (d != (short) i) {
			printf("Error at %d: Val: %d (should be %d)", i, d, (short) i);
		}
		if (!(i % (SDRAM_LEN/16))) {
			printf("Get Progress: %d/%d\n", i, SDRAM_LEN);
		}
	}

	for (int i = 0; i < 10; i++) {
		sdram.put(i, i*5);
	}

	// Since short is signed, it we should get
	// -6, -5, -4... 0, 1, 2, 3
	for (int i = 10; i < 20; i++) {
		sdram.put(i, 65520+i);
	}

	for (int i = 0; i < 20; i++) {
		printf("%4d, %5d\n", i, sdram.get(i));
	}
}

void test_sdram_waveform_player(void) {

	int len = 1024;

	waveformplayer.stop();

	// Test correct read and write to SDRAM from ARM
	for (unsigned int i = 0; i < len; i++) {
		sdram.put(i, i*2);
	}

	for (unsigned int i = 0; i < len; i++) {
		short d = sdram.get(i);
		if (d != (short)(i*2)) {
			printf("Error at %d: Val: %d (should be %d)", i, d, (short)(i*2));
		}
	}

	// Now load a square wave
	for (unsigned int i = 0; i < len; i++) {
		if (i % 8 > 3) {
			sdram.put(i, 0xFFFF);
		} else {
			sdram.put(i, 0);
		}
	}

	waveformplayer.setlen(len);
	waveformplayer.start();

}

void sine_wave_waveform_player(void) {

	while (1) {
		if (switches.newval()) {

			unsigned int freq = switches.get();

			waveformplayer.stop();

			freq = freq << 4; //no use to have waves < 20hz

			printf("New frequency: %d Hz\n", freq);

			unsigned int len = 48000 * 1000 / freq;

			if (freq > 0) {

				for(unsigned int i = 0; i < len; i++) {
					sdram.put(i, ((short)(0xFFFF * sin(2*pi*i*freq/48000))) & 0xFFFF);
					//printf("%04x, %f\n", (short)(0xFFFF * sin(2*pi*i*freq/48000)), 2*pi*i*freq/48000);
				}

				printf("Data loaded into SDRAM\n");

				waveformplayer.setlen(len);
				waveformplayer.start();
			}
		}
	}

}

/*
void test_parser(void) {
	int len;
	char *data;
	Parser parser(&bluetooth);

	while (1) {
		len = parser.getdata(&data);
		*//*if(len) {
			for (int i = 0; i < len; i++) {
				printf("%c", data[i]);
			}
		}*//*
	}
}
*/
