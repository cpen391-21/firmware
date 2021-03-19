#ifndef AUDIO_H
#define AUDIO_H


// memory mapped audio core

class AudioCore {
    private:
        volatile unsigned int *control_address;
        volatile unsigned int *fifospace_address;
        volatile unsigned int *ldata_address;
        volatile unsigned int *rdata_address;
    public:
        AudioCore(unsigned int address);

        // It is up to the user to put 16, 20, or 24
        // bit data to match the qsys configuration.
        void putmono(int data);
        void putleft(int data);
        void putright(int data);

        unsigned int get_min_fifospace();
        unsigned int get_l_fifospace();
        unsigned int get_r_fifospace();

        void clear_fifo();
};


#endif // AUDIO_H
