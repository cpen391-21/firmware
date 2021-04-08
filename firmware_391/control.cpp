#include "control.h"
#include "parser.h"
#include "signal_gen.h"
#include <time.h>

Parser                  parser(&bluetooth);

Control::Control(){
    this->playing = false;
    this->duration = 0.0;
    parser.reset_bt_parser();
}

/*
* constantly looping executing commands
*/
int Control::commence(){
    int parse_result;
    while(true){ // always loop
        if (!this->update_timer()) {
            this->stop_player();
        }
        parse_result = parser.increment_parser(&(this->command)); // increment parser and if a command is produced, execute it
        if (parse_result >= 0){
            this->execute_cmd(this->command);

            /*
            * respond to command
            */ 
            if(this->command.cmd == DURATION) {
                this->send_duration_str();
            }
            else {
                bluetooth.sendmsg(parser.parse_buf);
            }
        }
    }
}

/*
* simple switch statement to route commands
*/
int Control::execute_cmd(struct bt_command cmd){
    switch(cmd.cmd) {
        case NEW_WAVE:
            this->duration = (clock_t) cmd.param1;
            this->elapsed = 0.0;
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
            this->start_player();
            break;
        case STOP_WAVE:
            this->stop_player();
            break;
        case PAUSE:
            this->stop_player();
            break;
        case RESUME:
            this->start_player();
            break;
        case DURATION:
            break;
        default: break;
    }

    return -1;
}

/*
* takes in components and makes a waveform_element out of them
*/
struct waveform_element Control::assign_periodic(waveform_t type, double freq, double amplitude, double offset) {
    struct waveform_element wf_element;
    wf_element.type = type;
    wf_element.periodic.freq = freq;
    wf_element.periodic.amplitude = amplitude;
    wf_element.periodic.offset = offset;

    return wf_element;
}

/*
* takes in components and makes a waveform_element out of them
*/
struct waveform_element Control::assign_simple(waveform_t type, double amplitude) {
    struct waveform_element wf_element;
    wf_element.type = type;
    wf_element.simple.amplitude = amplitude;

    return wf_element;
}

/*
* starts player with timeout protection and keeps track of playing status and time
*/
bool Control::start_player() {
    if (this->update_timer()) {
        waveformplayer.start();
        this->playing = true;
        return true;
    }
    waveformplayer.stop();
    this->playing = false;
    return false;
}

/*
* stops player and updates playing status and time
*/
void Control::stop_player() {
    waveformplayer.stop();
    this->playing = false;
}

/*
* changes the values of elapsed to represent time elapsed playing.
* needs to be called often for acurrate timeout/duration control
*/
bool Control::update_timer() {
    clock_t curr;
    curr = clock();
    if (this->playing) {
        this->elapsed += curr - this->last_clock;
    }
    this->last_clock = curr;

    return this->elapsed < this->duration; // true if we still have time left
}

int Control::send_duration_str() {
    char msg[DUR_MSG_LEN];

    clock_t remaining_clk = this->duration - this->elapsed;
    double remaining = (double) remaining_clk;

    sprintf(msg, DUR_MSG_START, remaining);

    return parser.send_str_bt(msg);
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
