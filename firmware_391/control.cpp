#include "control.h"
#include <time.h> 

Parser                  parser(&bluetooth);

control::control(){
    this->playing = false;
    parser.reset_bt_parser();
}

int control::commence(){
    char* data;
    int parse_result;

    while(true){
        parse_result = parser.parse_bluetooth(&(this->command));
        if (parse_result >= 0){
            this->execute_cmd(this->command);
            // struct waveform_element el = {.type = sine, .periodic = {.freq = 40, .amplitude = 1, .offset = 90}};
            // waveforms[i++] = el;
        }
        else {
            if (this->playing){
                // TODO: check for stop command in switches
                continue;
            }
            else {
                // TODO: maybe add switch input?
                continue;
            }
        }
    }
}


int control::execute_cmd(struct bt_command cmd){
    switch(cmd.cmd) {
        default: break;
    }

    return -1;
}


/*
int control::stream_audio(char* initial, unsigned int len){
    unsigned int offset = 0;
    unsigned int i;

    char data[STREAM_BUF_LEN];
    short putval;
    unsigned int data_len;

    time_t last_recv, curr_time;

    for(i=0; i < len - 1; i+=2, offset+=2){
        putval = *(short *)(&(data[i]));
        sdram.put(offset, putval);
    }

    if (len % 2){ // half a 16 bit word has been sent
        data[0] = data[len-1];
        data_len = parser.getdata(&(data[1]));
        data_len ++;
    }
    else {
        data_len = parser.getdata(&data);
    }
    
    while(data_len > 1 || difftime(curr_time, recv_time) < STREAM_TIMEOUT){
        if (data_len > 1){
            for(i=0; i < data_len - 1; i+=2, offset+=2){
                putval = *(short *)(&(data[i]));
                sdram.put(offset, putval);
            }
        }
        if (data_len % 2){ // half a 16 bit word has been sent
            data[0] = data[len-1];
            data_len = parser.getdata(&(data[1]));
            data_len ++;
            if (data_len > 1) { // check if we've received and update timeout
                time(&recv_time);
            }
        }
        else {
            data_len = parser.getdata(&data);
            if (data_len > 0) { // check if we've received and update timeout
                time(&recv_time);
            }
        }
        time(&curr_time);
    }
    return offset;
}
*/
