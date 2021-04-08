#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include "rs232.h"

#define STARTFLAG '+'
#define ENDFLAG   '\r'
#define ESCAPE    0x10
#define BUFLEN    1024

enum command_t {
    NEW_WAVE,
    ADD_SINE,
    ADD_RANDOM,
    ADD_SQUARE,
    ADD_OFFSET,
    START_WAVE,
    STOP_WAVE
};



class Parser {
    private:
        RS232 *rs232;
        char buffer[BUFLEN];
    public:
        Parser(RS232 *rs232);

        // Copies a datagram into data.
        // Returns the length of said datagram.
        int getstring(char **data);

        int startflag();
};

#endif //PARSER_H
