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
#include "sdram.h"
#include "waveform_player.h"
#include "control.h"

#include "tests.h"

Switches             switches(0xFF200000);
RS232               bluetooth(0xFF200080);
WaveformPlayer waveformplayer(0xFF200090);
SDRAM                   sdram(0xC0000000);

int main(void) {
	Control control;
	bluetooth.flush();
	control.commence();
}
