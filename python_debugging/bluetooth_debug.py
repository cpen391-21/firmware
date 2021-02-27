from bluetooth import *
from pprint import pprint
import arm_comms_pb2 as pb2


i = pb2.bt_interrupt()
i.command = pb2.Mode.start_custom
i.intended_duration = 120
i.frequency = 5.34

print(i)

datagram = i.SerializeToString()

j = pb2.bt_interrupt()
j.ParseFromString(datagram)
print(j)

devices = discover_devices()

# our HC-05 has mac address 20:18:11:20:28:19
print(devices)