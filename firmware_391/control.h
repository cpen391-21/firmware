#ifndef CONTROL_H
#define CONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "audio.h"
#include "rs232.h"
#include "switches.h"
#include "waveform_player.h"
#include "sdram.h"
#include "signal_gen.h"

#define STREAM_BUF_LEN  4096
#define READ_BUF_LEN    4096

#define STREAM_TIMEOUT  5

#define WAVEFORM_ARRAY_SIZE 32

extern Switches switches;
extern RS232 bluetooth;
extern WaveformPlayer waveformplayer;
extern SDRAM sdram;

typedef enum {
    NEW_WAVE,
    ADD_SINE,
    ADD_TRIANGLE,
    ADD_RANDOM,
    ADD_SQUARE,
    ADD_OFFSET,
    START_WAVE,
    STOP_WAVE,
    PAUSE,
    RESUME,
    DURATION
} command_t;

typedef enum {
    sine,
    noise,
    square,
    triangle,
    offset
} waveform_t;

struct bt_command{
    command_t cmd;
    double param1;
    double param2;
    double param3;
};

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
        bool start_player();
        void stop_player();

        bt_command command;
        char read_buf[READ_BUF_LEN];
        unsigned int cmd_start;
        bool playing;

        // time values in seconds
        clock_t duration;
        clock_t elapsed;

        clock_t last_clock;
    public:
        control();
        int commence();
        int execute_cmd(struct bt_command cmd);
        struct waveform_element assign_periodic(waveform_t type, double freq, double amplitude, double offset);
        struct waveform_element assign_simple(waveform_t type, double amplitude);

        struct waveform_element waveforms[WAVEFORM_ARRAY_SIZE];
        unsigned int waveforms_i;
};

#endif
