#include "control.h"

int control::stream_audio(){
    unsigned int offset = 0;
    unsigned int i;

    char data[STREAM_BUF_LEN];
    unsigned int datagram_len = this->parser.get_data(&data);
    short putval;

    while(datagram_len > 0){
        for(i=0; i < datagram_len; i+=2, offset+=2){
            putval = *(short *)(&(data[i]));
            this->sdram.put(offset, putval);
        }
    }
    return offset;
}

int control::commence(){
    char data[READ_BUF_LEN];
    unsigned int datagram_len;

    while(true){
        datagram_len = this->parser.get_data(&data); // start every 
        if (datagram_len == 0){
            if (state == IDLE){
                // TODO: maybe add switch input?
                continue;
            }
            else {
                // TODO: check for stop command in switches
                continue;
            }
        }

        // TODO check datagram for commands
        if(curr_state == IDLE){ // check we're starting stream TODO: check it's a start stream command
            this->stream_audio();
        }
    }
}