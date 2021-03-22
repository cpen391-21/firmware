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
#include <math.h>

#include "audio.h"
#include "rs232.h"
#include "parser.h"
#include "switches.h"
#include "waveform_player.h"

Switches             switches(0xFF200000);
RS232               bluetooth(0xFF200080);
WaveformPlayer waveformplayer(0xFF200090);

int main(void) {

	while (1) {
		if (switches.newval()) {
			printf("New Switch val: %02X\n", switches.get());
			waveformplayer.stop();
			waveformplayer.setlen(switches.get());
			waveformplayer.start();
		}
	}
}
