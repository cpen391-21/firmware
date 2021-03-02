#include <string>
#include <stdlib.h>
#include "rs232.h"
#include "wifi.h"

wifi::wifi(unsigned int address = DEFAULT_ADDRESS)
{
    this->uart = new RS232(address);
}

int wifi::send_cmd(std::string cmd){
    uart->putchar('A');
    uart->putchar('T');
    // send command
    uart->putchar('\r');
}

int wifi::connect(std::string SSID, std::string password){
    // concat passwors and string into command
    // send command
    // check if OK sent or not
}

void wifi::set_protocol(enum net_protocol protocol)
{
    this->protocol = protocol;
}

enum net_protocol wifi::get_protocol()
{
    return this->protocol;
}