#ifndef RS232_H
#define RS232_H



class RS232 {
    private:
        unsigned int *address;
    public:
        RS232() {};
        RS232(unsigned int *address) {};

        int putchar(char c);
        int getchar(void);
        int received_data();
        void flush();
};
/*
int putcharBluetooth(char c);

int BluetoothGetChar(void);


int BluetoothReceivedData(void);

void BluetoothFlush(void);
*/

#endif // RS232_H
