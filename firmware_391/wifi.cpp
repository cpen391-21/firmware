#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h> 
#include "rs232.h"
#include "wifi.h"

wifi::wifi(unsigned int address = DEFAULT_ADDRESS)
{
    this->uart = new RS232(address);
}

std::string wifi::send_cmd(std::string cmd){
    time_t start, curr_time;
    std::vector<char> resp_vec; // use char vector to receive response dynamically
    char resp_char;

    uart->putchar('A'); // all AT commands start with "AT+"
    uart->putchar('T');
    uart->putchar('+');

    for (char c: cmd){
        uart->putchar(c);
    }
    uart->putchar('\r'); // return ends AT command

    time(&start);
    while (!uart->received_data()){ // wait for esp to respond
        time(&curr_time);
        if (difftime(curr_time, start) > RESP_TIMEOUT) {
            return RESP_TIMEOUT_MSG;
        }
    }

    while (uart->received_data()){
        uart->getchar(&resp_char);
        resp_vec.push_back(resp_char);
    }

    std::string resp(resp_vec.begin(), resp_vec.end()); // convert char vector to string
    return resp;
}

int wifi::connect(std::string SSID, std::string password){
    std::string cmd = WIFI_CONNECT + SSID + "\",\"" + password + "\"";
    std::string resp = this->send_cmd(cmd);

    return resp.compare(0, 2, "OK"); // check that the ESP sent back "OK"
    // TODO parse and evaluate error messages to return better error codes
}

void wifi::set_protocol(enum net_protocol protocol)
{
    this->protocol = protocol;
}

enum net_protocol wifi::get_protocol()
{
    return this->protocol;
}