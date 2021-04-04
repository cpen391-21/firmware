#ifndef SIGNAL_GEN_H
#define SIGNAL_GEN_H

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <cmath>

#include "control.h"
#include "sdram.h"

#define MIN_SAMPLES 1024

#define SAMPLING_RATE 48000

#define pi 3.141592653589793238

#define MIN -32767
#define MAX  32767

class SignalGen {
	private:
		static int round(double val);
    public:
        static short getvalue(struct waveform_element *el, unsigned int i);
        static void write_waveforms(struct waveform_element *arr, SDRAM *sdram);
};

#endif /* SIGNAL_GEN_H */
