#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include "rs232.h"
#include "control.h"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

#define STARTFLAG 0x40
#define ENDFLAG   0x41
#define ESCAPE    0x10
#define BUFLEN    256

class Parser {
    private:
        RS232 *rs232;
        int startflag();
        int compare_string_start(string compstr, string refstring);
        struct waveform_element parse_command(string command, int type, int num_doubles);

        string start_kw;
        string stop_kw;
        string sine_kw;
        string square_kw;
        string triangle_kw;
        string rand_kw;
        string pause_kw;
        string resume_kw;

    public:
        Parser(RS232 *rs232);

        char buffer[BUFLEN];
        int size;

        // Copies a datagram into data.
        // Returns the length of said datagram.
        int getstring();
        struct waveform_element parse_string();

};

#endif //PARSER_H
