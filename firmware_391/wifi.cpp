#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h> 
#include "rs232.h"
#include "wifi.h"
#include "audio.h"

wifi::wifi(unsigned int address)
{
    this->protocol = TCP;
    this->cw_mode = DEFAULT_MODE;
    this->uart = new RS232(address);
}

std::string wifi::send_cmd(std::string cmd){
    time_t start, curr_time;
    std::vector<char> resp_vec; // use char vector to receive response dynamically
    char resp_char;

    uart->putchar('A'); // all AT commands start with "AT+"
    uart->putchar('T');
    uart->putchar('+');

    for (int i = 0; i < cmd.size(); i++){
        uart->putchar(cmd[i]);
    }
    uart->putchar('\r'); // return ends AT command

    time(&start);
    while (!uart->read_fifo_size()){ // wait for esp to respond
        time(&curr_time);
        if (difftime(curr_time, start) > RESP_TIMEOUT) {
            return RESP_TIMEOUT_MSG;
        }
    }

    while (uart->read_fifo_size()){
        uart->getchar(&resp_char);
        resp_vec.push_back(resp_char);
    }

    std::string resp(resp_vec.begin(), resp_vec.end()); // convert char vector to string
    return resp;
}

int connect(char* ssid, char* password){
    /*
    std::string cmd = WIFI_CONNECT + ssid + "\",\"" + password + "\"";
    std::string resp = this->send_cmd(cmd);
    return resp.compare(0, 2, "OK"); // check that the ESP sent back "OK"
    */
   return 0;
    // TODO parse and evaluate error messages to return better error codes
}

double wifi::ping(std::string address){
    std::string cmd = PING + address + "\"";
    std::string resp = this->send_cmd(cmd);
    std::size_t end = resp.find("\r");
    std::size_t start = 6; // index of first part of ping in ms

    if(resp.find("OK") != std::string::npos) {
        return std::atof(resp.substr(start, end - start).c_str()) / 1000.0;
    }
    return 0.0;
}

void wifi::set_protocol(enum net_protocol protocol)
{
    this->protocol = protocol;
}

enum net_protocol wifi::get_protocol()
{
    return this->protocol;
}
