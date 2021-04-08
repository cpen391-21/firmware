/*
 * tests.cpp
 *
 *  Created on: Mar 22, 2021
 *      Author: Tyler
 */

#include "tests.h"

AudioCore               audio(0xFF200090);

char outbuffer[256];

short        audio_data[AUDIO_DATA_MAX_SIZE];
unsigned int audio_data_size = 1;
unsigned int audio_data_position = 0;

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

// Demo for waveform_player and signal_gen
struct waveform_element waveforms[WAVEFORM_ARRAY_SIZE];
int num_waveforms;

void clear_waveform_elements() {
	struct waveform_element nothing = {noise, {0}};

	for (int i = 0; i < WAVEFORM_ARRAY_SIZE; i++) {
		waveforms[i] = nothing;
	}

	num_waveforms = 0;

	//SignalGen::write_waveforms(waveforms, &sdram);
	//printf("Cleared waveform elements\n");
}

void signal_gen_sine_440(void) {
	unsigned int num_samples;

	struct waveform_element wave0 = {sine, {440, 0.9}};
	//struct waveform_element wave1 = {sine, {110, 0.3}};

    waveforms[0] = wave0;
	//waveforms[1] = wave1;

	num_samples = SignalGen::write_waveforms(waveforms, &sdram);
	printf("Sine wave written (440Hz) ");
	printf("(Num samples: %d)\n", num_samples);

	waveformplayer.setlen(num_samples);
}

void signal_gen_sine_587(void) {
	unsigned int num_samples;

	struct waveform_element wave0 = {sine, {587.33, 0.9}};
	//struct waveform_element wave1 = {sine, {110, 0.3}};

    waveforms[0] = wave0;
	//waveforms[1] = wave1;

	num_samples = SignalGen::write_waveforms(waveforms, &sdram);
	printf("Sine wave written (587.33Hz) ");
	printf("(Num samples: %d)\n", num_samples);

	waveformplayer.setlen(num_samples);
}

void signal_gen_fourth(void) {
	unsigned int num_samples;

	struct waveform_element wave0 = {sine, {523.25, 0.45}};
	struct waveform_element wave1 = {sine, {698.46, 0.45}};

    waveforms[0] = wave0;
	waveforms[1] = wave1;

	num_samples = SignalGen::write_waveforms(waveforms, &sdram);
	printf("Perfect fourth (523.25Hz/698.46Hz) ");
	printf("(Num samples: %d)\n", num_samples);

	waveformplayer.setlen(num_samples);
}

void noisy_square(void) {
	unsigned int num_samples;

	struct waveform_element wave0 = {square, {440, 0.45}};
	struct waveform_element wave1 = {noise, {0.45}};

    waveforms[0] = wave0;
	waveforms[1] = wave1;

	num_samples = SignalGen::write_waveforms(waveforms, &sdram);
	printf("Noisy Square (440Hz) ");
	printf("(Num samples: %d)\n", num_samples);

	waveformplayer.setlen(num_samples);
}


void signal_gen_noise(void) {
	unsigned int num_samples;

	struct waveform_element rand_noise = {noise, {1./32}};

	for (int i = 0; i < WAVEFORM_ARRAY_SIZE; i++) {
		waveforms[i] = rand_noise;
	}

	num_samples = SignalGen::write_waveforms(waveforms, &sdram);
	printf("Noise data written ");
	printf("(Num samples: %d)\n", num_samples);
}


void waveform_player_demo(void) {
	unsigned int val;
	while (true) {
		if (switches.newval()) {

			waveformplayer.stop();

			clear_waveform_elements();

			val = switches.get();
			printf("New Switch Value: %d\n", val);

			switch(val) {
				case 1: signal_gen_noise();
						break;
				case 2: signal_gen_sine_440();
						break;
				case 4: signal_gen_sine_587();
						break;
				case 8: signal_gen_fourth();
						break;
				case 16: noisy_square();
						break;
				default: continue;
			}


			waveformplayer.start();

		}
	}
}

void new_parser(void) {
	int len;

	Parser parser(&bluetooth);

	while (1) {

		len = parser.getstring();

		if (len) {
			struct waveform_element el = parser.parse_string();

			printf("Type: %d\n", el.type);
			printf("First val %f\n", el.periodic.freq);

			bluetooth.sendmsg(parser.buffer, len);
		}
	}
}


void controller(void) {
	int len;
	int num_samples;

	double time_remaining = 0;
	double lasttime;
	bool playing = false;

	Parser parser(&bluetooth);

	while (1) {

		len = parser.getstring();

		if (len) {
			struct waveform_element el = parser.parse_string();
			bluetooth.sendmsg(parser.buffer, len);

			switch (el.type) {
				case stop:
					clear_waveform_elements();
					waveformplayer.stop();
					playing = false;
					printf("Stopped\n");
					break;

				case sine:
				case noise:
				case square:
				case triangle:
					waveforms[++num_waveforms] = el;
					printf("New wave added\n");
					break;

				case start:
					num_samples = SignalGen::write_waveforms(waveforms, &sdram);
					printf("New data written to SDRAM ");
					printf("(num samples: %d)\n", num_samples);
					waveformplayer.setlen(num_samples);
					time_remaining = el.simple.amplitude;
					lasttime = (double)clock()/(double)CLOCKS_PER_SEC;

					printf("Playing waveform for %f seconds\n", time_remaining);
					waveformplayer.start();
					playing = true;
					break;

				case pause:
					waveformplayer.stop();
					playing = false;
					printf("Paused\n");
					break;
				case resume:
					waveformplayer.start();
					playing = true;
					printf("Playing. Time remaining: %f seconds\n", time_remaining);
					break;
			}
		}

		if (playing) {
			double newtime = (double)clock()/(double)CLOCKS_PER_SEC;
			time_remaining -= (newtime - lasttime);
			lasttime = newtime;

			if (time_remaining <= 0) {
				printf("Out of time. Stopping Waveform\n");
				waveformplayer.stop();
				playing = false;
			}
		}
	}
}