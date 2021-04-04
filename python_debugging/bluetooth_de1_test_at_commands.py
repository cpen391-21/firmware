
# This file is meant to emulate the phone app for validating the DE-1's
# handling of AT commands. Maybe we send them some gibberish as well.

# This is the list of commands which the DE1 should be able to respond to.
# All commands expect a response. If there is no response within the timeout
# period, send it again!

commands = []

commands.append("EN+STOP_WAVE\n")

commands.append("EN+NEW_WAVE,100\n")

commands.append("EN+ADD_RANDOM,0.1\n")

commands.append("EN+ADD_SQUARE,40.5,0.5\n")

commands.append("EN+ADD_OFFSET,12\n")

commands.append("EN+START_WAVE\n")

commands.append("EN+PAUSE\n")

commands.append("EN+RESUME\n")

# This will return the remaining duration of the DE1 regimen.
commands.append("EN+DURATION?\n")

commands.append("Gibberish")

for c in commands:
    print(c, end='')



import serial, time


# Serial loopback for testing this circuit
#s = serial.serial_for_url('loop://', 115200, timeout=1)

# Connect to the DE-1 over bluetooth, and change the COM port to match
# the port of your computer (see "Bluetooth_working_with_USB_uart.png"
# for an example of this).
s = serial.Serial('COM6', 115200, timeout=1)

print("Serial connected")

i = 0

while True:
    command_bytes = commands[i].encode('utf-8')
    #s.write("fake news".encode('utf-8'))
    s.write(command_bytes)

    lns = s.readlines()

    if (len(lns) == 0 or lns[0] != command_bytes):
        print("Error! We did not receive what we sent. Sent:")
        print(command_bytes.decode('utf-8'))
        print("Received:")
    else:
        i = (i+1) % len(commands)

    for ln in lns:
        print(ln.decode('utf-8'),end='')
