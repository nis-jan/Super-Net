import struct
import time
import threading

sem = threading.Semaphore()

import serial

magicpattern = "SuperNetSerial"

def msg_callback(msg: str, sender: int):
    print(f"[INCOMING]: {sender} says:\n\t{msg}")

def success_callback(ref: int):
    print(f"[SENDSUCCESFULL]: Packet with reference " + str(ref) + " was sent succesfully")

def error_callback(ref: int):
    print(f"[SENDERROR]: Packet with reference " + str(ref) + "couldn't be received")


def interpret_incoming(device: serial.Serial):
    print("thread gestartet")
    while True:
        """sem.acquire()
        if device.in_waiting > 0:
            try:
                print(device.read(device.in_waiting).decode())
            except:
                print("!")
        sem.release()
        time.sleep(0.1)
        continue"""
        sem.acquire()
        if device.in_waiting > 0: print("data available")
        if(wait_for_magicpattern(device)):
            opcode = get_byte(device)[0]
            if(opcode == 2):
                sender = get_int(device)
                msg_length = get_int(device)
                msg = get_bytes(device, msg_length)
                msg_callback(msg.decode(), sender)
            if(opcode == 3):
                ref = get_int(device)
                success_callback(ref)
            if(opcode == 4):
                ref = get_int(device)
        sem.release()
        time.sleep(0.4)





def get_byte(device):
    timeout = 100
    while timeout > 0:
        data = device.read(1)
        if (len(data) > 0):
            return data
        timeout -= 1
        time.sleep(0.01)
    return [-1]

def get_bytes(device, count):
    msg = bytearray(b'')
    for i in range(count):
        msg.append(get_byte(device)[0])
    return msg

def get_int(device):
    a = []
    ret = 0
    for i in range(0, 4):
        ret += (int(get_byte(device)[0]) << i * 8)

    return int(ret)

def send_int(device, a: int):
    device.write(a.to_bytes(4,'little'))

def send_msg(device, _msg: str, recipient: int):
    sem.acquire()

    send_magicpattern(device)               # magicpattern
    time.sleep(0.2)
    device.write((1).to_bytes(1,'little'))
    send_int(device, recipient)             # recipient
    msg = _msg.encode()
    send_int(device, len(msg))              # msg length
    device.write(msg)                       # msg
    time.sleep(0.2)
    ref = get_int(device)
    sem.release()
    return ref

def set_addr(device, adress: int):
    print("[*] setting adress!")
    send_magicpattern(device)
    device.write((5).to_bytes(1,'little'))
    send_int(device, adress)

def send_magicpattern(device):
    msg = magicpattern.encode()
    device.write(msg)

# wait for magic pattern returns true if the magicpattern was received
# otherwise it returns false
def wait_for_magicpattern(device):
    iterations = 0
    while (iterations < 4):
        pattern_received = True
        for i in range(len(magicpattern)):
            incoming = get_byte(device)[0]
            if incoming == -1:
                break
            #print(f"{incoming} == {magicpattern[i].encode()[0]}?")
            if int(magicpattern[i].encode()[0]) != incoming:
                pattern_received = False
                break
        if pattern_received == True:
            return True

    return False
