import serial
import serial.tools.list_ports
import threading
import sendrcv



def chose_device():
    ports = serial.tools.list_ports.comports()
    print("chose device: ")
    for i in range(len(ports)):
        print(f"\t({i+1}): {ports[i][0]}: {ports[i][1]}")
    choice = input("choice: ")
    port = ports[int(choice)-1][0]
    device = serial.Serial(port, baudrate=9600, timeout=.1)
    return device



commands = ["Send Message", "Change Address"]

def menu():
    print("chose what to do:")
    for i in range(2):
        print(f"\t({i+1}): {commands[i]}")
    choice = input("choice: ")
    return int(choice)




# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    dev = chose_device()
    reading_thread = threading.Thread(target=sendrcv.interpret_incoming, args=[dev])
    reading_thread.start()
    while(True):
        choice = menu()
        if choice == 1:
            recipient = input("Message to: ")
            message = input("Message: ")
            ref = sendrcv.send_msg(dev,  message, int(recipient))
            print(f"packet sent with ref: {ref}")
        elif choice == 2:
            new_addr = int(input("New Address: "))
            sendrcv.set_addr(dev, new_addr)


    dev.close()


