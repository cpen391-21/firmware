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
#define PARSE_BUF_LEN   4096

#define STREAM_TIMEOUT  5

#define PREFIX          "BT+"
#define PREFIX_LEN      3
#define TERMINATION     '\r'

enum command {START, STP, STREAM};
/* what the parser expects for the next part of the in progress command: 
* PREF: rest of prefix
* CMD: rest of command text (start, stop)
* PARAM1A: pre-decimal part of param1
* PARAM1B: post-decimal part of param1
* etc for params 2 and 3
* parser is also looking for termoination character, commas or periods
*/
enum command_part{PREF, CMD, PARAM1A, PARAM1B, PARAM2A, PARAM2B, PARAM3A, PARAM3B}


class control{
    private:
        Switches             switches(0xFF200000);
        RS232               bluetooth(0xFF200080);
        // RS232                    wifi(0xFF200088);
        WaveformPlayer waveformplayer(0xFF200090);
        SDRAM                   sdram(0xC0000000);
        Parser                  parser(&bluetooth);

        bt_command last_command;
        bt_command in_progress;


        char read_buf[READ_BUF_LEN];
        char parse_buf[PARSE_BUF_LEN];
        unsigned int parse_buf_i;
        unsigned int cmd_start;

        bool playing;
        int stream_audio(char* initial, unsigned int len);

    public:
        control();
        int commence();
}

struct bt_command{
    command cmd;
    double param1;
    double param2;
    double param3;
};