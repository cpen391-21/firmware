#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIMEOUT 1024

char command_strs[NUM_COMMANDS][MAX_CMD_LEN] = {
    "NEW_WAVE",
    "ADD_SINE",
    "ADD_TRIANGLE",
    "ADD_RANDOM",
    "ADD_SQUARE",
    "ADD_OFFSET",
    "START_WAVE",
    "STOP_WAVE",
    "PAUSE",
    "RESUME",
    "DURATION?"
};

Parser::Parser(RS232 *rs232) {
    this->rs232 = rs232;
}

/*
* resets all parser memory. Must be done for every command
*/
void Parser::reset_bt_parser(){
    // TODO move prev command to another buffer for echoing
    this->parse_buf_i = 0;
    this->state = PREF;
    this->in_progress.param1 = 0.0;
    this->in_progress.param2 = 0.0;
    this->in_progress.param3 = 0.0;
}

/*
* checks if there's a char waiting and if so, calls parse_bluetooth_char
*/
int Parser::increment_parser(bt_command *cmd){
    char c;
    if (rs232->read_fifo_size()){
        rs232->getchar(&c);
        return this->parse_bluetooth_char(c, cmd);
    }
    else {
        return NO_CHAR;
    }
}

/*
* parses a single char for commands
*/
int Parser::parse_bluetooth_char(char c, bt_command *cmd){
    static unsigned int parse_denom = 1;
    int chr_int;
    int cmd_id;

    if (c == TERMINATION) { // end of message found
        this->parse_buf[this->parse_buf_i] = c;
        if (this->state == PREF) { // terminating before full command send
            cmd_id = INVALID_PREF;
        }
        else if (this->state == CMD){ // terminating without params
            cmd_id = this->check_cmd_str(this->parse_buf, PREFIX_LEN, this->parse_buf_i - PREFIX_LEN);
        }
        else { // terminated with some params added
            cmd_id = (int) this->in_progress.cmd;
        }
        if (cmd_id >= 0) *cmd = this->in_progress;
        this->reset_bt_parser();
        return cmd_id;
    }

    switch(this->state){ // state machine parsing
        case PREF:
            if (c == PREFIX[this->parse_buf_i]) {
                if (this->parse_buf_i >= PREFIX_LEN - 1) this->state = CMD;
            }
            else {
                this->reset_bt_parser();
                return INVALID_PREF;
            }
            break;
        case CMD:
            if (c == ',') { // we've found end of cmd string
                cmd_id = this->check_cmd_str(this->parse_buf, PREFIX_LEN, this->parse_buf_i - PREFIX_LEN);
                if (cmd_id < 0 || cmd_id >= NUM_COMMANDS){
                    this->reset_bt_parser();
                    return INVALID_CMD;
                }
                else{
                    this->in_progress.cmd = (command_t) cmd_id;
                    this->state = PARAM1A;
                }
            }
            break;
        case PARAM1A:
            parse_denom = 1;
            chr_int = char_to_int(c);
            if(c == '.') { // start of decimal portion
                this->state = PARAM1B;
            }
            else if(chr_int >= 0) { // simply adding another digit
                this->in_progress.param1 *= 10;
                this->in_progress.param1 += chr_int;
            }
            else if (c == ',') { // end of number
                this->state = PARAM2A;
            }
            break;
        case PARAM1B:
            chr_int = char_to_int(c);
            if(chr_int >= 0) {
                parse_denom *= 10;
                this->in_progress.param1 += ((double) chr_int) / parse_denom;
            }
            else if (c == ',') {
                this->state = PARAM2A;
            }
            break;

        case PARAM2A:
            parse_denom = 1;
            chr_int = char_to_int(c);
            if(c == '.') {
                this->state = PARAM2B;
            }
            else if(chr_int >= 0) {
                this->in_progress.param2 *= 10;
                this->in_progress.param2 += chr_int;
            }
            else if (c == ',') {
                this->state = PARAM3A;
            }
            break;
        case PARAM2B:
            chr_int = char_to_int(c);
            if(chr_int >= 0) {
                parse_denom *= 10;
                this->in_progress.param2 += ((double) chr_int) / parse_denom;
            }
            else if (c == ',') {
                this->state = PARAM3A;
            }
            break;
        case PARAM3A:
            parse_denom = 1;
            chr_int = char_to_int(c);
            if(c == '.') {
                this->state = PARAM3B;
            }
            else if(chr_int >= 0) {
                this->in_progress.param3 *= 10;
                this->in_progress.param3 += chr_int;
            }
            else if (c == ',') {
                this->state = GIB;
            }
            break;
        case PARAM3B:
            chr_int = char_to_int(c);
            if(chr_int >= 0) {
                parse_denom *= 10;
                this->in_progress.param3 += ((double) chr_int) / parse_denom;
            }
            else if (c == ',') {
                this->state = GIB;
            }
            break;
        default:
            break;
    }

    this->parse_buf[this->parse_buf_i] = c;
    this->parse_buf_i ++;

    return CMD_IN_PROG; // if we've gotten to this point without returning, it means we're midway through parsing a command
}

int Parser::check_cmd_str(char* str, unsigned int start, unsigned int len){
    int cmd = -1;
    if (len > MAX_CMD_LEN) len = MAX_CMD_LEN; // really shouldn't happen
    
    for(int i = 0; i < NUM_COMMANDS; i++){
        cmd = i;
        for(int j = 0; j < len; j++){
            if (str[j+start]!= command_strs[i][j]) { // found inequality
                cmd = -1;
                break;
            }
            if (str[j+start] == '\0') break;
        }
        if (cmd > -1) break;
    }
    return cmd;
}

/* 
* returns the int value for a single digit char or -1 for a period or -2 for neither
*/
int char_to_int(char c){
    switch(c) {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case '.': return -1;
    }
    return -2;
}
