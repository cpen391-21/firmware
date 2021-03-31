#ifndef PARSER_H
#define PARSER_H

#include "rs232.h"

#define STARTFLAG 0x40
#define ENDFLAG   0x41
#define ESCAPE    0x10
#define BUFLEN    4096

#define STOP        0
#define START_SINE  1
#define START_DC    2
#define START_CUST  3
#define AUDIO_STRM  4

class Parser {
    private:
        RS232 *rs232;
        char buffer[BUFLEN];
    public:
        Parser(RS232 *rs232);

        // Copies a datagram into data.
        // Returns the length of said datagram.
        int getdata(char **data);

        int startflag();
};

struct cmd_message {
    uint8_t command;
    int intended_duration;
    double frequency;
    double amplitude;
};

// sent periodically to confirm operation on phone app.
struct bt_response { 
    uint8_t status;
    double impedance;
    double total_duration; 
    int device_id;
};

struct logistical_data {
    uint8_t status;
    int intended_duration;
    int total_duration;
};

/*
message bt_interrupt {
    required enum command { start_sine, start_dc, start_custom, stop };
    required int32 intended_duration;
    optional double frequency;
    optional double amplitude;
    repeated byte audiodata;
} 

// sent periodically to confirm operation on phone app.
message bt_response { 
    required enum status { sine, dc, custom, stopped };
    required double impedance;
    required double total_duration; 
    required int32 device_id;
}

Message logistical_data {
    required enum status { sine, dc, custom, stopped };
    required int32 intended_duration;
    required int32 total_duration;
}
*/

#endif //PARSER_H
