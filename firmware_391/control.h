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
enum state{
    IDLE, // not outputting anything (possibly loading)
    PLAY_DC,
    PLAY_SINE,
    PLAY_CUSTOM
    };

class control{
    private:
        Switches             switches(0xFF200000);
        RS232               bluetooth(0xFF200080);
        // RS232                    wifi(0xFF200088);
        WaveformPlayer waveformplayer(0xFF200090);
        SDRAM                   sdram(0xC0000000);
        Parser                  parser(&bluetooth);

        state curr_state;

        int stream_audio();

    public:
        control();
        int commence();
}