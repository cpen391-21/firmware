#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIMEOUT 1024

Parser::Parser(RS232 *rs232) {
    this->rs232 = rs232;
}

int Parser::startflag() {
	char c;
	rs232->getchar(&c);

	if (c == STARTFLAG) {
		return 1;
	}
	return 0;
}

int Parser::getdata(char **data) {
	// We want to make sure that if there was data in our address, we free it
	// To avoid introducing a memory leak
	free(*data);

    char c;
    int i = 0;
    bool previous_escape = false;

    /*
    do {
        rs232->getchar(&c);
        i++;
    } while (c != STARTFLAG && i < TIMEOUT);

    if (i == TIMEOUT) {
    	return 0;
    }*/

    i = 0;
    while (i < BUFLEN) {
    	while(rs232->getchar(&c)) {};

        /* If the previous character was an escape character, this current
         * character is part of the data. That means that we add it to our
         * buffer regardless of whether or not it is a special character */
        if (previous_escape) {
            buffer[i] = c;
            i++;
            previous_escape = false;
        }

        /* This character is an escape, but the previous character wasn't, so
         * don't add this to the buffer and instead just mark that there was an
         * escape. */
        else if (c == ESCAPE) {
            previous_escape = true;
        }

        /* STARTFLAG without previous_escape is bad news in a message. We are
         * probably parsing this message incorrectly if a new message is coming
         * in right now. We should start over. */
        else if (c == STARTFLAG) {
            i = 0;
            previous_escape = false;
        }

        /* All the data we need is already in the buffer */
        else if (c == ENDFLAG) {
            break;
        }

        /* This is just a normal character, so we add it to the buffer. */
        else {
            buffer[i] = c;
            i++;
            previous_escape = false;
        }
    }

    if (i == 256) {
        printf("Buffer overflow!\n");
    }

    *data = (char *)malloc(i*sizeof(char));
    if (*data == NULL) {
    	return 0;
    }

    memcpy(*data, buffer, i);
    return i;
}
