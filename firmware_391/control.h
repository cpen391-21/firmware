#ifndef CONTROL_H
#define CONTROL_H

class control{
    private:
        Switches             switches(0xFF200000);
        RS232               bluetooth(0xFF200080);
        // RS232               wifi(0xFF200080);
        WaveformPlayer waveformplayer(0xFF200090);
        SDRAM                   sdram(0xC0000000);

    public:
        control();
        int commence();
}