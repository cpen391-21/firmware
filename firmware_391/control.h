#ifndef CONTROL_H
#define CONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "audio.h"
#include "rs232.h"
#include "parser.h"
#include "switches.h"
#include "waveform_player.h"
#include "sdram.h"

#define STREAM_BUF_LEN  4096
#define READ_BUF_LEN    4096

#define STREAM_TIMEOUT  5

#define WAVEFORM_ARRAY_SIZE 32

extern Switches switches;
extern RS232 bluetooth;
extern WaveformPlayer waveformplayer;
extern SDRAM sdram;

typedef enum {
    sine,
    noise,
    square,
    triangle,
    offset
} waveform_t;

// For sine, random, square
struct periodic_command {
	double freq;
	double amplitude;
	double offset;
};

struct simple_command {
	double amplitude;
};

struct waveform_element {
	waveform_t type;
	
	union {
		struct periodic_command periodic;
		struct simple_command simple;
    };
};	

class control{
    private:
        int stream_audio(char* initial, unsigned int len);

        bt_command command;
        char read_buf[READ_BUF_LEN];
        unsigned int cmd_start;
        bool playing;
    public:
        control();
        int commence();
        int execute_cmd(struct bt_command cmd);
        struct waveform_element waveforms[WAVEFORM_ARRAY_SIZE];
};

#endif
