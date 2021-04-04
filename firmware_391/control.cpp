#include "control.h"
#include <time.h> 

void control::control(){
    this->playing = false;
    this->reset_bt_parser();
}

int control::stream_audio(char* initial, unsigned int len){
    unsigned int offset = 0;
    unsigned int i;

    char data[STREAM_BUF_LEN];
    short putval;
    unsigned int data_len;

    time_t last_recv, curr_time;

    for(i=0; i < len - 1; i+=2, offset+=2){
        putval = *(short *)(&(data[i]));
        this->sdram.put(offset, putval);
    }

    if (len % 2){ // half a 16 bit word has been sent
        data[0] = data[len-1];
        data_len = this->parser.get_data(&(data[1]));
        data_len ++;
    }
    else {
        data_len = this->parser.get_data(&data);
    }
    
    while(data_len > 1 || difftime(curr_time, recv_time) < STREAM_TIMEOUT){
        if (data_len > 1){
            for(i=0; i < data_len - 1; i+=2, offset+=2){
                putval = *(short *)(&(data[i]));
                this->sdram.put(offset, putval);
            }
        }
        if (data_len % 2){ // half a 16 bit word has been sent
            data[0] = data[len-1];
            data_len = this->parser.get_data(&(data[1]));
            data_len ++;
            if (data_len > 1) { // check if we've received and update timeout
                time(&recv_time);
            }
        }
        else {
            data_len = this->parser.get_data(&data);
            if (data_len > 0) { // check if we've received and update timeout
                time(&recv_time);
            }
        }
        time(&curr_time);
    }
    return offset;
}

int control::commence(){
    char data[READ_BUF_LEN];
    unsigned int data_len;

    while(true){
        data_len = this->parser.get_data(&data); // start every 
        if (data_len == 0){
            if (this->playing){
                // TODO: check for stop command in switches
                continue;
            }
            else {
                // TODO: maybe add switch input?
                continue;
            }
        }
        else {
            
        }

        // TODO check datagram for commands
        if(curr_state == IDLE){ // check we're starting stream TODO: check it's a start stream command
            this->stream_audio();
        }
    }
}

void control::reset_bt_parser(){
    this->parse_buf_i = 0;
    this->command_part = PREF;
    this->in_progress.param1 = 0.0;
    this->in_progress.param2 = 0.0;
    this->in_progress.param3 = 0.0;
}

int control::parse_bluetooth(char c){
    static unsigned int parse_denom = 1;
    int chr_int;
    int cmd_id;

    if (c == TERMINATION) { // end of message found
        this->parse_buf[this->parse_buf_i] = c;
        if (this->command_part == PREF) { // terminating before full command send
            cmd_id = INVALID_PREF;
        }
        else if (this->command_part == CMD){ // terminating without params
            cmd_id = this->check_cmd_str(this->parse_buf, PREFIX_LEN, this->parse_buf_i - PREFIX_LEN)
        }
        else { // terminated with some params added
            cmd_id = (int) this->in_progress.cmd;
        }
        this->reset_bt_parser();
        return cmd_id;
    }

    switch(this->command_part){
        case PREF:
            if (c == PREFIX[this->parse_buf_i]) {
                this->parse_buf[this->parse_buf_i] = c;
                this->parse_buf_i ++;
                if (this->parse_buf_i >= PREFIX_LEN) this->command_part = CMD;
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
                    this->command_part = PARAM1A;
                }
            }
            break;
        case PARAM1A:
            parse_denom = 1;
            chr_int = char_to_int(c);
            if(c == '.') { // start of decimal portion
                this->command_part = PARAM1B;
            }
            else if(chr_int >= 0) { // simply adding another digit
                this->in_progress.param1 *= 10;
                this->in_progress.param1 += chr_int;
            }
            else if (c == ',') { // end of number
                this->command_part = PARAM2A;
            }
            break;
        case PARAM1B:
            chr_int = char_to_int(c);
            if(chr_int >= 0) {
                parse_denom *= 10;
                this->in_progress.param1 += (double) chr_int / parse_denom;
            }
            else if (c == ',') {
                this->command_part = PARAM2A;
            }
            break;

        case PARAM2A:
            parse_denom = 1;
            chr_int = char_to_int(c);
            if(c == '.') {
                this->command_part = PARAM2B;
            }
            else if(chr_int >= 0) {
                this->in_progress.param2 *= 10;
                this->in_progress.param2 += chr_int;
            }
            else if (c == ',') {
                this->command_part = PARAM3A;
            }
            break;
        case PARAM2B:
            chr_int = char_to_int(c);
            if(chr_int >= 0) {
                parse_denom *= 10;
                this->in_progress.param2 += (double) chr_int / parse_denom;
            }
            else if (c == ',') {
                this->command_part = PARAM3A;
            }
            break;
        case PARAM3A:
            parse_denom = 1;
            chr_int = char_to_int(c);
            if(c == '.') {
                this->command_part = PARAM3B;
            }
            else if(chr_int >= 0) {
                this->in_progress.param3 *= 10;
                this->in_progress.param3 += chr_int;
            }
            else if (c == ',') {
                this->command_part = GIB;
            }
            break;
        case PARAM3B:
            chr_int = char_to_int(c);
            if(chr_int >= 0) {
                parse_denom *= 10;
                this->in_progress.param3 += (double) chr_int / parse_denom;
            }
            else if (c == ',') {
                this->command_part = GIB;
            }
            break;
        default:
            break;
    }

    if(this->command_part != PREV) {
        this->parse_buf[this->parse_buf_i] = c;
        this->parse_buf_i ++;
    }

    return CMD_IN_PROG; // if we've gotten to this point without returning, it means we're midway through parsing a command
}

int control::check_cmd_str(char* str, unsigned int start, unsigned int len){
    int cmd = -1;
    if (len > MAX_CMD_LEN) len = MAX_CMD_LEN; // really shouldn't happen
    
    for(int i = 0; i < NUM_COMMANDS; i++){
        cmd = i;
        for(int j = 0; j < len; j++){
            if (str[i][j+start] != command_strs[i][j]) { // found inequality
                cmd = -1;
                break;
            }
            if (str[i][j+start] == '\0') break;
        }
        if (cmd > -1) break;
    }
    return cmd;
}

int control::execute_cmd(struct command cmd){
    switch(cmd.cmd) {
        NEW_WAVE:
    }
}

/* 
* returns the int value for a single digit char or -1 for a period or -2 for neither
*/
int char_to_int(char c){
    switch(c) {
        case '0'; return 0;
        case '1': return 1;
        case '2'; return 2;
        case '3'; return 3;
        case '4'; return 4;
        case '5'; return 5;
        case '6'; return 6;
        case '7'; return 7;
        case '8'; return 8;
        case '9'; return 9;
        case '.'; return -1;
    }
    return -2;
}