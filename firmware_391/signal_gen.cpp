#include "signal_gen.h"

short SignalGen::getvalue(struct waveform_element *el, unsigned int i) {
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

int SignalGen::round(double val) {
    return (val > 0.0) ? std::floor(val + 0.5) : std::ceil(val - 0.5);
}

void SignalGen::write_waveforms(struct waveform_element *arr, SDRAM *sdram) {
    double min_freq = DBL_MAX;
    double product_freq = 1;

    struct waveform_element *minsize = arr;

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

        struct waveform_element *element = arr;

        for (unsigned int j = 0; j < WAVEFORM_ARRAY_SIZE; j++) {
            value += getvalue(element, i);
            element++;
        }

        sdram->put(i,value);
    }
}
