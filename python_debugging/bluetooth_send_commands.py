from pprint import pprint
import serial
import time
import serial.tools.list_ports as port_list
import math

s = serial.Serial('COM6', 115200, timeout=0.2)
print("Serial connected")

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

def start_audio(duration):
    bytestr = bytearray("new," + str(duration), 'ascii')
    framebytestring(bytestr)
    return bytestr

def stop_audio():
    bytestr = bytearray("stop", 'ascii')
    framebytestring(bytestr)
    return bytestr

def pause_audio():
    bytestr = bytearray("ps", 'ascii')
    framebytestring(bytestr)
    return bytestr

def play_audio():
    bytestr = bytearray("rs", 'ascii')
    framebytestring(bytestr)
    return bytestr

def sin(freq, amp):
    bytestr = bytearray("si," + str(freq) + "," + str(amp), 'ascii')
    framebytestring(bytestr)
    return bytestr

def square(freq, amp):
    bytestr = bytearray("sq," + str(freq) + "," + str(amp), 'ascii')
    framebytestring(bytestr)
    return bytestr

def triangle(freq, amp):
    bytestr = bytearray("tr," + str(freq) + "," + str(amp), 'ascii')
    framebytestring(bytestr)
    return bytestr

def noise(amp):
    bytestr = bytearray("ra," + str(amp), 'ascii')
    framebytestring(bytestr)
    return bytestr

def sendcommand(bytes):
    global s
    s.write(bytes)
    lns = s.readlines()
    for ln in lns:
        print(ln)


def demo_noise():
    print("Noise Demo")
    sendcommand(stop_audio())
    sendcommand(noise(0.9))
    sendcommand(start_audio(10))
    time.sleep(5)
    sendcommand(pause_audio())
    time.sleep(2)
    sendcommand(play_audio())
    time.sleep(7)

def demo_beats():
    print("Beats Demo")
    sendcommand(stop_audio())
    sendcommand(sin(440,0.5))
    sendcommand(sin(445,0.5))
    sendcommand(start_audio(5))
    time.sleep(8)

def demo_chord():
    print("Fourth Demo")
    sendcommand(stop_audio())
    sendcommand(sin(523.25,0.40))
    sendcommand(sin(698.46,0.40))
    sendcommand(start_audio(5))
    time.sleep(8)

def demo_noisy_triangle():
    print("Noisy Triangle Demo")
    sendcommand(stop_audio())
    sendcommand(triangle(200,0.40))
    sendcommand(noise(0.40))
    sendcommand(start_audio(5))
    time.sleep(8)



if __name__ == "__main__":
    sendcommand(stop_audio())
    time.sleep(1)

    demo_noise()
    demo_beats()
    demo_noisy_triangle()