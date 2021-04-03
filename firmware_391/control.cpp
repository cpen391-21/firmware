#include "control.h"
#include <time.h> 

void control::control(){
    this->playing = false;
    this->parse_buf_i = 0;
    this->cmd_start = 0;
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

int control::parse_bluetooth(char c){
    if (c == TERMINATION) {
        if (this->command_part == CMD){
            // check what command this is
        }
        if (this->command_part != PREF){
            // return command
        }
    }
    switch(this->command_part){
        case PREF:
            if (c == PREFIX[this->parse_buf_i]) {
                this->parse_buf[this->parse_buf_i] = c;
                this->parse_buf_i ++;
                if (this->parse_buf_i >= PREFIX_LEN) this->command_part = CMD;
            }
            else {
                this->parse_buf_i = 0;
            }
        case CMD:
            if (c == ',') {
                // check what command this is
            }
            else {
                this->parse_buf[this->parse_buf_i] = c;
                this->parse_buf_i ++;
            }
    }
}