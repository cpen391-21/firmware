#ifndef RS232_H
#define RS232_H



class RS232 {
    private:
        volatile unsigned int *address;
        volatile unsigned int *control_address;
    public:
        RS232(unsigned int address);

        void sendmsg(char *c);
        void sendmsg(char *c, int len);

        int putchar(char c);
        int getchar(char *c);
        void flush();
};
/*
int putcharBluetooth(char c);

int BluetoothGetChar(void);


int BluetoothReceivedData(void);

void BluetoothFlush(void);
*/

#endif // RS232_H
