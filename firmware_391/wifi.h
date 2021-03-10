#include <string>
#include <stdlib.h>
#include "rs232.h"

/* CW MODES */
#define STATION_MODE    1 // client
#define AP_MODE         2 // host
#define DUAL_MODE       3 // both
#define DEFAULT_MODE    DUAL_MODE

#define DEFAULT_ADDRESS 0xFF200088

#define WIFI_CONNECT    "CWJAP=\""
#define TCP_CONNECT     "CIPSTART=\"TCP\",\""
#define SEND            "CIPSEND="
#define DISCONNECT      "CIPCLOSE"
#define PING            "PING=\""

#define RESP_TIMEOUT        2.0
#define RESP_TIMEOUT_MSG    "send_cmd() timed out without response"

enum net_protocol {TCP, UDP};

class wifi {
    private:
        RS232 *uart;
        enum net_protocol protocol;
        unsigned int cw_mode;

        /*
        * sends the given command and returns the response string
        */
        std::string send_cmd(std::string cmd);
        int set_cw_mode(unsigned int cwmode);
    public:
        wifi(unsigned int address = DEFAULT_ADDRESS);

        /*
        * connect to a given wifi network
        */
        int connect(char* ssid, char* password);

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
};
