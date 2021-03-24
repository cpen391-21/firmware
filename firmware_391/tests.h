/*
 * tests.h
 *
 *  Created on: Mar 22, 2021
 *      Author: Tyler
 */

#ifndef TESTS_H_
#define TESTS_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "audio.h"
#include "rs232.h"
#include "parser.h"
#include "switches.h"
#include "waveform_player.h"
#include "sdram.h"

extern Switches switches;
extern RS232 bluetooth;
extern WaveformPlayer waveformplayer;
extern SDRAM sdram;

/* Half of 16 bits */
#define amplitude 0x7FFD;

/* Big enough for 1 second of audio */
#define AUDIO_DATA_MAX_SIZE 48000

#define SDRAM_LEN 33554432


// Plays a square wave repeatedly to the memory-mapped audio core
void test_audio_square();

// Sets up the audio_data array with a sine wave
void setup_audio_sine();

// Plays data in the audio_data array (requires setup_audio_sine)
void test_audio_sine();

// Sends example strings over bluetooth and loopbacks data
void test_bluetooth();

//
void test_parser(void);

// Returns 0 if strings are equal up to a null character from either
// of the strings, else returns -1
int compare_string_start(const char *s1, const char *s2);

// If there is space on the FIFO queue, send data to the memory mapped
// Audio core and update the audio_data_position
void send_mono_audio();

// Parses datastring into 24-bit address and 16-bit audio data,
// Then updates the array at the address.
void update_audio_data(char *datastring);

// Sets the size of the waveform that we are playing (we will wrap around
// After we hit max_size)
void update_size(char *datastring);

// For debugging
void print_byte_array(char *bytearr, int len);

// Tyler's Module 2 demo. Will listen for bytestring commands
// (start/stop/data/set size) and control the memory-mapped audio
// player using bluetooth.
void mono_bt_player(void);

// Write to the SDRAM and read back the data.
void test_sdram(void);


#endif /* TESTS_H_ */
