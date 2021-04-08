
# This file is meant to emulate the phone app for validating the DE-1's
# handling of AT commands. Maybe we send them some gibberish as well.

# This is the list of commands which the DE1 should be able to respond to.
# All commands expect a response. If there is no response within the timeout
# period, send it again!

commands = []

commands.append("EN+STOP_WAVE\r\r")

commands.append("EN+NEW_WAVE,100a\r\r")

commands.append("EN+ADD_RANDOM,0.1a\r\r")

commands.append("EN+ADD_SQUARE,40.5,0.5a\r\r")

commands.append("EN+ADD_OFFSET,12a\r\r")

commands.append("EN+START_WAVE\r\r")

commands.append("EN+PAUSE\r\r")

commands.append("EN+RESUME\r\r")

# This will return the remaining duration of the DE1 regimen.
commands.append("EN+DURATION?\r\r")

commands.append("Gibberish")

for c in commands:
    print(c)




import serial, time


# Serial loopback for testing this circuit
#s = serial.serial_for_url('loop://', 115200, timeout=1)

# Connect to the DE-1 over bluetooth, and change the COM port to match
# the port of your computer (see "Bluetooth_working_with_USB_uart.png"
# for an example of this).
s = serial.Serial('COM3', 115200, timeout=1)

print("Serial connected")

i = 0

while True:
    print("Sending command: " + commands[i])
    command_bytes = commands[i].encode('ascii')
    #s.write("fake news".encode('utf-8'))
    s.write(command_bytes)

    lns = s.readlines()
    if (len(lns) == 0 or lns[0][0:-1] != command_bytes):
        print("Error! We did not receive what we sent. Sent:")
        print(command_bytes.decode('ascii'))
        print("Received:")
    else:
        print("Success, received: ")
        
    i = (i+1) % len(commands)
    for ln in lns:
        try:
            print(ln.decode('ascii'),end='')
        except:
            print(ln,end='')

    time.sleep(5)

