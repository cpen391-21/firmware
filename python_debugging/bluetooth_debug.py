from pprint import pprint
import serial
import time
import serial.tools.list_ports as port_list
import arm_comms_pb2 as pb2

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

#i = pb2.bt_interrupt()
#i.command = pb2.Mode.start_custom
#i.intended_duration = 120
#i.frequency = 5.34

#datagram = i.SerializeToString()

#datagramb = bytearray(datagram)

#for d in datagramb:
    #print(f"{d:02x},", end=" ")
#print()

#framebytestring(datagramb)

#for d in datagramb:
    #print(f"{d:02x},", end=" ")
#print()

#datagramb = unframebytestring(datagramb)

#for d in datagramb:
    #print(f"{d:02x},", end=" ")
#print()
# j = pb2.bt_interrupt()
# j.ParseFromString(datagram)
# print(j)

bytestr = bytearray("Parsed Data\nLooks\nLike\nThis!\n\n", 'ascii')
framebytestring(bytestr)
for b in bytestr:
    print(f"{b:02x}", end=' ')
print()

s = serial.Serial('COM6', 38400)

while True:
    s.write(bytestr[0:5])
    time.sleep(1)
    s.write(bytestr[5:])
    d = s.read_all()
    print(d)
    time.sleep(2)