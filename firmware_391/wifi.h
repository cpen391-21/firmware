#include <string>
#include <stdlib.h>
#include "rs232.h"

/* CW MODES */
#define STATION_MODE    1 // client
#define AP_MODE         2 // host
#define DUAL_MODE       3 // both

enum net_protocol {TCP, UDP};

class wifi {
    private:
        RS232 uart;
        enum net_protocol protocol;
        unsigned int cwmode;
        int send_cmd(std::string cmd);
        int set_cwmode(unsigned int cwmode);
    public:
        wifi(unsigned int address);
        wifi();

        /*
        * connect to a given wifi network
        */
        int connect(std::string SSID, std::string password);

        /*
        * returns True if a packet is waiting in the buffer and False otherwise
        */
        bool packet_waiting();

        /*
        * returns the packet size and places a pointer to the packet in the packet pointer.
        * caller is responsible for freeing packet memory
        */
        unsigned int receive_packet(char **packet);

        /*
        * sends the packet of given size to the given address. Returns 0 on success or error code otherwise
        */
        int send_data(unsigned int size, char *data, std::string address);

        /*
        * pings the given server and returns the RTT in seconds if successful or -1 otherwise
        */
        double ping(std::string address);

        void set_protocol(enum net_protocol protocol);
        enum net_protocol get_protocol();
}