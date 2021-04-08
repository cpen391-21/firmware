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
#define DUR_MSG_LEN     64

#define STREAM_TIMEOUT  5

#define WAVEFORM_ARRAY_SIZE 32

#define DUR_MSG_START   "EN+DURATION=%.2f\n"

#define TEST_CONTROL    1

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

class Control{
    private:
        /*
        * start player (with timeout check)
        */
        bool start_player();

        /*
        * stop player and update playing tracking
        */ 
        void stop_player();

        /*
        * updates duration and returns whether or not we still have time left (bool)
        */ 
        bool update_timer();

        bt_command command;
        char read_buf[READ_BUF_LEN];
        unsigned int cmd_start;
        bool playing;

        // time values in seconds
        clock_t duration; // given duration
        clock_t elapsed; // time elapsed treating
        clock_t last_clock; // last time measured

        int stream_audio(char* initial, unsigned int len);
    public:
        Control();
        /*
        * primary control loop. should run indefinitely
        */
        int commence();

        /*
        * executes the given command once parsed. Assumes parser still
        * has blank command text in buffer for echoing
        */ 
        int execute_cmd(struct bt_command cmd);

        /*
        * builds a waveform_element struct with a periodic command
        */ 
        struct waveform_element assign_periodic(waveform_t type, double freq, double amplitude, double offset);

        /*
        * builds a waveform_element struct with a simple command
        */ 
        struct waveform_element assign_simple(waveform_t type, double amplitude);

        /*
        * forms and sends a string responsding to a duration request
        */
        int send_duration_str();

        struct waveform_element waveforms[WAVEFORM_ARRAY_SIZE];
        unsigned int waveforms_i;
};

#endif
