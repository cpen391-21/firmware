#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIMEOUT 1024 * 1024

Parser::Parser(RS232 *rs232) {
    this->rs232 = rs232;

    start_kw = "new";
    stop_kw = "stop";
    sine_kw = "si";
    square_kw = "sq";
    rand_kw = "ra";
}

int Parser::startflag() {
	char c;
	rs232->getchar(&c);

	if (c == STARTFLAG) {
		return 1;
	}
	return 0;
}

int Parser::getstring() {

    char c;
    int i = 0;
    int j = 0;
    bool previous_escape = false;

    do {
    	i = rs232->getchar(&c);
    } while (i && c != STARTFLAG);

    if (c != STARTFLAG) {
    	return 0;
    }

    i = 0;

    while (i < BUFLEN) {
    	while(!rs232->getchar(&c)) {j++;};

        if (j == TIMEOUT) {
            printf("Timeout!\n");
            break;
        }

        //printf("%c", c);

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
        if (c == STARTFLAG) {
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

    if (i == BUFLEN) {
        printf("Buffer overflow!\n");
        rs232->flush();
    }

    this->size = i;
    return i;
}

struct waveform_element Parser::parse_string() {
    if (!size) {
        printf("Error: Length 0. Return\n");
    }

    if (size > 25) {
        printf("Error: String too long. Return\n");
    }

    string compstr(buffer);


    if (!compare_string_start(compstr, start_kw)) {
        return parse_command(compstr, start, 1);
    } else if (!compare_string_start(compstr, stop_kw)) {
        return parse_command(compstr, stop, 0);
    } else if (!compare_string_start(compstr, sine_kw)) {
        return parse_command(compstr, sine, 2);
    } else if (!compare_string_start(compstr, square_kw)) {
        return parse_command(compstr, square, 2);
    } else if (!compare_string_start(compstr, rand_kw)) {
        return parse_command(compstr, noise, 1);
    } else {
        struct waveform_element l = {sine, {0, 0}};
        return l;
    }
}

struct waveform_element Parser::parse_command(string command, int type, int num_doubles) {
    stringstream ss(command);
    vector<string> split;

    double d1;
    double d2;

    struct waveform_element el;

    while (ss.good()) {
        string substr;
        getline(ss, substr, ',');
        split.push_back(substr);
    }

    for (std::size_t i = 0; i < split.size(); i++) {
        cout << split[i] << endl;
    }

    if (num_doubles > 0) {
        d1 = strtod(split[1].c_str(), NULL);
    }

    if (num_doubles > 1) {
        d2 = strtod(split[2].c_str(), NULL);
    }

    switch(num_doubles) {
        case 0: struct waveform_element e = {(waveform_t)type, {0}};
                el = e;
                break;
        case 1: struct waveform_element f = {(waveform_t)type, {d1}};
                el = f;
                break;
        case 2: struct waveform_element g = {(waveform_t)type, {d1, d2}};
                el = g;
                break;
    }

    return el;
}

int Parser::compare_string_start(string compstr, string refstring) {
    int len = refstring.length();

    string substring = compstr.substr(0,len);
	return substring.compare(refstring);
}