from pprint import pprint
import serial
import time
import serial.tools.list_ports as port_list
import math

#https://eli.thegreenplace.net/2009/08/12/framing-in-serial-communications/
def framebytestring(bytestring):
    startflag = 0x40
    endflag   = 0x41
    escape    = 0x10

    bytestring[0:0] = bytes([startflag])
    i = 1
    while i < len(bytestring):
        if bytestring[i] == startflag or \
           bytestring[i] == escape    or \
           bytestring[i] == endflag:
                bytestring[i:i] = bytes([escape])
                i += 1
        i += 1
    bytestring.append(endflag)

def unframebytestring(bytestring):
    startflag = 0x40
    endflag   = 0x41
    escape    = 0x10

    newarr = bytearray()

    i = 0
    previous_escape = False
    while i < len(bytestring):
        if bytestring[i] == startflag:
            break
        i += 1

    if i == len(bytestring):
        return

    i += 1
    while i < len(bytestring):
        if previous_escape:
            newarr.append(bytestring[i])
            previous_escape = False
        elif bytestring[i] == escape:
            previous_escape = True
        elif bytestring[i] == endflag or bytestring[i] == startflag:
            break
        else:
            newarr.append(bytestring[i])
            previous_escape = False
        i += 1

    return newarr

def printbytearray(ba):
    for b in ba:
        print(f"{b:02x}", end=' ')
    print()

# Audio data must be 16 bits!
def createDatagram(audio_data, address):
    audio_data_bytes = audio_data.to_bytes(2, 'big', signed=True)
    address_bytes = address.to_bytes(3, 'big')

    bytestr = bytearray("d:", 'ascii')
    bytestr.extend(audio_data_bytes)
    bytestr.extend(address_bytes)

    framebytestring(bytestr)
    #printbytearray(bytestr)
    return bytestr

def update_size(size):
    bytestr = bytearray("s:", 'ascii')
    bytestr.extend(size.to_bytes(3, 'big'))
    framebytestring(bytestr)
    return bytestr

def start_audio():
    bytestr = bytearray("start", 'ascii')
    framebytestring(bytestr)
    return bytestr

def stop_audio():
    bytestr = bytearray("stop", 'ascii')
    framebytestring(bytestr)
    return bytestr

s = serial.Serial('COM6', 115200)
print("Serial connected")

command_retry_amount = 10
retry_amount = 1

for i in range(command_retry_amount):
    s.write(stop_audio())

print("Begin transmitting data")
time.sleep(3)

for i in range(1024):
    aud = int(math.sin(2*math.pi*i/512)*65535/2)
    for j in range(retry_amount):
        s.write(createDatagram(aud, i))
        time.sleep(0.02)
    print(s.read_all().decode("utf-8"))

print("Done transmitting data")
time.sleep(1)
for i in range(command_retry_amount):
    s.write(update_size(1024))
print(s.read_all())
time.sleep(0.1)

for i in range(command_retry_amount):
    s.write(start_audio())

while True:
    time.sleep(1)
    print(s.read_all())

