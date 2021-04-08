#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include "rs232.h"
#include "control.h"

#define BUFLEN    4096

#define PREFIX          "EN+"
#define PREFIX_LEN      3
#define TERMINATION     '\r'

#define NUM_COMMANDS    11
#define MAX_CMD_LEN     16

#define CMD_IN_PROG     -1
#define INVALID_PREF    -2
#define INVALID_CMD     -3
#define NO_CHAR         -4

/* what the parser expects for the next part of the in progress command: 
* PREF: rest of prefix
* CMD: rest of command text (start, stop)
* PARAM1A: pre-decimal part of param1
* PARAM1B: post-decimal part of param1
* etc for params 2 and 3
* parser is also looking for termoination character, commas or periods
*/
enum command_part{PREF, CMD, PARAM1A, PARAM1B, PARAM2A, PARAM2B, PARAM3A, PARAM3B, GIB};

class Parser {
    private:
        RS232 *rs232;

        bt_command in_progress;
        command_part state;

        unsigned int parse_buf_i;
    public:
        Parser(RS232 *rs232);

        /*
        * checks the given string and returns its command ID or -1 if it's not a command
        */
        int check_cmd_str(char* str, unsigned int start, unsigned int len);

        /*
        * 'increments' parser by getting one char from bluetooth if possibe and parsing it
        */
        int increment_parser(bt_command *cmd);

        /*
        * parses a char. result will be >= 0 if a command is put in cmd or <0 otherwise
        * check error/progress codes above
        */
        int parse_bluetooth_char(char c, bt_command *cmd);

        /*
        * clears out parser
        */ 
        void reset_bt_parser();

        /*
        * sends '\0' terminated char array over bluetooth. used for echoing commands.
        * returns the number of characters sent
        */
        int send_str_bt(char *str);

        char parse_buf[BUFLEN];
};

/* 
* returns the int value for a single digit char or -1 for a period or -2 for neither
*/
int char_to_int(char c);

#endif //PARSER_H
