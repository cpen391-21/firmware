#ifndef PARSER_H
#define PARSER_H

#include "rs232.h"

#define STARTFLAG 0x40
#define ENDFLAG   0x41
#define ESCAPE    0x10
#define BUFLEN    256

class Parser {
    private:
        RS232 *rs232;
        char buffer[BUFLEN];
    public:
        Parser(RS232 *rs232);

        // Copies a datagram into data.
        // Returns the length of said datagram.
        int getdata(char **data);
};

#endif //PARSER_H
