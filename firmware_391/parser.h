#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include "rs232.h"

#define BUFLEN    4096

#define STOP        0
#define START_SINE  1
#define START_DC    2
#define START_CUST  3
#define AUDIO_STRM  4

#define PREFIX          "EN+"
#define PREFIX_LEN      3
#define TERMINATION     '\n'

#define NUM_COMMANDS    7
#define MAX_CMD_LEN     16

#define CMD_IN_PROG     -1
#define INVALID_PREF    -2
#define INVALID_CMD     -3
#define NO_CHAR         -4

enum command_t {
    NEW_WAVE,
    ADD_SINE,
    ADD_RANDOM,
    ADD_SQUARE,
    ADD_OFFSET,
    START_WAVE,
    STOP_WAVE
};

/* what the parser expects for the next part of the in progress command: 
* PREF: rest of prefix
* CMD: rest of command text (start, stop)
* PARAM1A: pre-decimal part of param1
* PARAM1B: post-decimal part of param1
* etc for params 2 and 3
* parser is also looking for termoination character, commas or periods
*/
enum command_part{PREF, CMD, PARAM1A, PARAM1B, PARAM2A, PARAM2B, PARAM3A, PARAM3B, GIB};

struct bt_command{
    command_t cmd;
    double param1;
    double param2;
    double param3;
};

class Parser {
    private:
        RS232 *rs232;
        char parse_buf[BUFLEN];

        bt_command in_progress;
        command_part state;

        unsigned int parse_buf_i;
    public:
        Parser(RS232 *rs232);
        int check_cmd_str(char* str, unsigned int start, unsigned int len);
        int parse_bluetooth(bt_command *cmd);
        void reset_bt_parser();
};

/* 
* returns the int value for a single digit char or -1 for a period or -2 for neither
*/
int char_to_int(char c);

#endif //PARSER_H
