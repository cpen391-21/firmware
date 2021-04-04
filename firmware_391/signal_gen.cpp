#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <math.h>

#define WAVEFORM_ARRAY_SIZE 32
#define MIN_SAMPLES 1024

#define SAMPLING_RATE 48000

#define pi 3.141592653589793238

#define MIN -32767
#define MAX  32767

typedef enum {
    sine,
    noise,
    square,
    triangle,
    offset
} waveform_t;

// For sine, random, square
struct periodic_command {
	double freq;
	double amplitude;
	double offset;
};

struct simple_command {
	double amplitude;
};

struct waveform_element {
	waveform_t type;
	
	union {
		struct periodic_command periodic;
		struct simple_command simple;
    };
};	

struct waveform_element waveforms[WAVEFORM_ARRAY_SIZE];

void sdram_put(unsigned int i, short value) {
    if (!(i%4)) {
        printf("%d, %d\n", i, value);
    }
}


short getvalue(struct waveform_element *el, unsigned int i) {
    switch(el->type) {
        case sine:
            if (el->periodic.amplitude) {
                double value = sin(2*pi*i*el->periodic.freq/SAMPLING_RATE);
                value *= el->periodic.amplitude;
                return (short)(value * SHRT_MAX);
            }
            break;

        case square:
            if (el->periodic.amplitude) {
                double rate = (double)SAMPLING_RATE / el->periodic.freq / 2;
                if ((int)(i / rate) % 2) {
                    return (short)(MAX*el->periodic.amplitude);

                } else {
                    return (short)(MIN*el->periodic.amplitude);

                }
            }
            break;

        case triangle:
            if (el->periodic.amplitude) {
                double num_samples = SAMPLING_RATE / el->periodic.freq;

                double height = el->periodic.amplitude * (i % (int)num_samples)
                                / num_samples;

                return (short)(height*2*SHRT_MAX-SHRT_MAX);
            }
            break;

        case noise:
            if (el->simple.amplitude) {
                short value = (rand() % SHRT_MAX) - SHRT_MAX;
                value = (short)(value * el->simple.amplitude);
                return value;
            }
            break;
    }

    return 0;
}


double write_waveforms(struct waveform_element *el) {
    double min_freq = DBL_MAX;
    double product_freq = 1;

    struct waveform_element *minsize = el;

    // Firstly get the minimum frequency.
    for (int i = 0; i < WAVEFORM_ARRAY_SIZE; i++) {
        if (minsize->type == sine ||
            minsize->type == square ||
            minsize->type == triangle) {

            double freq = minsize->periodic.freq;

            if (freq > 0 && freq < min_freq) {
                min_freq = freq;
            }

            if (freq > 0) {
                product_freq *= freq;
            }
        }

        minsize++;
    }
    
    // Now we have the minimum frequency. Determine how many samples we should
    // use to represent this data.
    double samples_f = 48000. / min_freq;
    
    while (samples_f < MIN_SAMPLES) {
        samples_f *= 2;
    }

    unsigned int samples_i = round(samples_f);

    // Now that we know how many samples to write, it is time to create them!
    for (unsigned int i = 0; i < samples_i; i++) {
        short value = 0;

        struct waveform_element *element = el;

        for (unsigned int j = 0; j < WAVEFORM_ARRAY_SIZE; j++) {
            value += getvalue(element, i);
            element++;
        }

        sdram_put(i,value);
    }

    return min_freq;
}

int main()
{
    waveforms[0] = (struct waveform_element){.type = square, .periodic = {.freq = 100, .amplitude = 0.5, .offset = 90}};
    waveforms[1] = (struct waveform_element){.type = sine, .periodic = {.freq = 800, .amplitude = 0.3, .offset = 90}};
    waveforms[2] = (struct waveform_element){.type = noise, .simple = {.amplitude = 0.2}};

    double product = write_waveforms(waveforms);
    printf("Product: %f", product);

    return 0;
}
