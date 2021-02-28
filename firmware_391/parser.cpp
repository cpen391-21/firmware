#include "parser.h"
#include <stdio.h>


Parser::Parser(RS232 *rs232) {
    this->rs232 = rs232;
}

int Parser::getdata(char *& data) {
    unsigned char c;
    int i = 0;
    int result;
    bool previous_escape = false;

    do {
        result = rs232->getchar(&c);
        if (!result) {
            printf("New Char: %02x\n", c);
        }
    } while (c != STARTFLAG);

    while (i < BUFLEN) {
        for (int j = 0; j < i; j++) {
            printf("%c", buffer[j]);
        }

        do {
        	result = rs232->getchar(&c);
        } while (result);

        if (previous_escape) {
            buffer[i] = c;
            i++;
            previous_escape = false;
        }

        else if (c == ESCAPE) {
            previous_escape = true;
        }

        else if (c == ENDFLAG) {
            break;
        }

        /* STARTFLAG without previous_escape is bad news in a message. We are
         * probbaly parsing this message incorrectly if a new message is coming
         * in right now. We should start over. */
        else if (c == STARTFLAG) {
            i = 0;
            previous_escape = false;
        }

        else {
            buffer[i] = c;
            i++;
            previous_escape = false;
        }
    }

    for (int j = 0; j < i; j++) {
        printf("%c", buffer[j]);
    }
    printf("\n");
    return i;
}
