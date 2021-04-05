#include "control.h"
#include "parser.h"
#include "signal_gen.h"
#include <time.h>

Parser                  parser(&bluetooth);

control::control(){
    this->playing = false;
    this->duration = 0.0;
    parser.reset_bt_parser();
}

int control::commence(){
    int parse_result;

    while(true){
        parse_result = parser.parse_bluetooth(&(this->command));
        if (parse_result >= 0){
            this->execute_cmd(this->command);
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
        case NEW_WAVE:
            this->duration = cmd.param1;
            // TODO clean elements array
            break;
        case ADD_SINE:
            this->waveforms[this->waveforms_i] = this->assign_periodic(sine, cmd.param1, cmd.param2, 0);
            this->waveforms_i = (this->waveforms_i + 1) % WAVEFORM_ARRAY_SIZE;
            break;
        case ADD_RANDOM:
            this->waveforms[this->waveforms_i] = this->assign_simple(square, cmd.param1);
            this->waveforms_i = (this->waveforms_i + 1) % WAVEFORM_ARRAY_SIZE;
            break;
        case ADD_SQUARE:
            this->waveforms[this->waveforms_i] = this->assign_periodic(square, cmd.param1, cmd.param2, 0);
            this->waveforms_i = (this->waveforms_i + 1) % WAVEFORM_ARRAY_SIZE;
            break;
        case ADD_TRIANGLE:
            this->waveforms[this->waveforms_i] = this->assign_periodic(triangle, cmd.param1, cmd.param2, 0);
            this->waveforms_i = (this->waveforms_i + 1) % WAVEFORM_ARRAY_SIZE;
            break;
        case ADD_OFFSET:
            break;
        case START_WAVE:
            SignalGen::write_waveforms(this->waveforms, &sdram);
            waveformplayer.start();
            break;
        case STOP_WAVE:
            waveformplayer.stop();
            break;
        case PAUSE:
            waveformplayer.stop();
            break;
        case RESUME:
            waveformplayer.start();
            break;
        case DURATION:
            break;
        default: break;
    }

    return -1;
}

struct waveform_element control::assign_periodic(waveform_t type, double freq, double amplitude, double offset) {
    struct waveform_element wf_element;
    wf_element.type = type;
    wf_element.periodic.freq = freq;
    wf_element.periodic.amplitude = amplitude;
    wf_element.periodic.offset = offset;

    return wf_element;
}

struct waveform_element control::assign_simple(waveform_t type, double amplitude) {
    struct waveform_element wf_element;
    wf_element.type = type;
    wf_element.simple.amplitude = amplitude;

    return wf_element;
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
