/*
 * waveform_player.h
 *
 *  Created on: Mar 19, 2021
 *      Author: Tyler
 */

#ifndef WAVEFORM_PLAYER_H_
#define WAVEFORM_PLAYER_H_

class WaveformPlayer {
	private:
		volatile unsigned int *len_address;
		volatile unsigned int *control_address;
	public:
		WaveformPlayer(unsigned int address);

		void setlen(unsigned int len);
		void start();
		void stop();
		void restart();
};



#endif /* WAVEFORM_PLAYER_H_ */
