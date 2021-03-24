/*
 * waveform_player.cpp
 *
 *  Created on: Mar 22, 2021
 *      Author: Tyler
 */

#include "waveform_player.h"

WaveformPlayer::WaveformPlayer (unsigned int address) {
	this->len_address = (volatile unsigned int *)address;
	this->control_address = (volatile unsigned int *)(address+4);
}

void WaveformPlayer::setlen(unsigned int len) {
	*len_address = len;
}

void WaveformPlayer::start() {
	*control_address = 0x01;
}

void WaveformPlayer::stop() {
	*control_address = 0x00;
}

void WaveformPlayer::restart() {
	stop();
	start();
}
