import serial     #pyserial
import serial.tools.list_ports
import time
from threading import Thread
import numpy as np
# from numpy import uint8, uint16, uint32
import binascii


np.seterr(all="ignore", over="ignore")
uint8 = np.uint8
#FUCKING WARNINGS!!!

import os
import sys

try:
    #go to file directory
    os.chdir(os.path.dirname(sys.argv[0]))
except:
    pass

import colorama
from colorama import Fore, Back, Style
colorama.init()
cred =      Fore.RED
cgreen =    Fore.GREEN
cblue =     Fore.BLUE
cyellow =   Fore.YELLOW
clyellow =  Fore.LIGHTYELLOW_EX
clcyan =    Fore.LIGHTCYAN_EX
clmag =     Fore.LIGHTMAGENTA_EX
clblue =    Fore.LIGHTBLUE_EX
creset =    Fore.RESET


####################
# CONFIG
####################

speed = 9600




head   = 0xFF
comm   = 0x01
data1  = 0x86
data2  = 0x00
data3  = 0x00
data4  = 0x00
data5  = 0x00
data6  = 0x00

crc = 0x79
# crc    = uint8(comm + data1 + data2 + data3 + data4 + data5 + data6 + data7 + data8 + data9 + data10 + data11 + data12 + data13 + data14 + data15)





def find_port(vid_pid: str = '0403:6001'):
    """
    :param vid_pid: VID and PID of the device (for stm32 it's '0483:5740')
    :return: port name in OS
    """
    # 0403:6001 - FT232

    device = None
    devices = []
    ports = serial.tools.list_ports.comports()
    for port in serial.tools.list_ports.comports():
        if vid_pid in port.hwid:
            device = port.device
        if device is not None:
            devices.append(device.__str__())
    return devices

port = find_port()[0]

def send_bin(ser, data):
    # print(Fore.GREEN, 'HOST => ', Fore.RESET, data)
    ser.write(data)

def read_data(ser, timeout=3):
    timestamp_sec = time.time()
    stp=0
    data = []
    while True:
        ###################################
        # wait data
        ###################################
        try:
            while ser.inWaiting() <=0 :
                time.sleep(0.00001)
                if timestamp_sec + timeout < time.time():
                    return None 
        except Exception as e:
            print(cred, "[TH] ERROR inWaiting:", creset, ser.name,"Error:",e)
            print(cred, "[TH] Port data: ", creset, ser)

            return None 
        ###################################
        # read data
        ###################################
        try:
            while ser.inWaiting() > 0 :
                data_byte = int.from_bytes(ser.read(size=1), "big")
                if stp == 0:
                    if data_byte == 0xFF:
                        stp = 1
                        continue

                if stp > 0:
                    data.append(data_byte)
                    stp += 1

                if stp > 8:
                    crc = uint8(0)
                    crc = data[0] + data[1] + data[2] + data[3] + data[4] + data[5] + data[6]
                    crc = crc & 0xff
                    crc = uint8(0xff - uint8(crc)) + 1
                    if crc == uint8(data[7]):
                        return data
                    else:
                        return False

        except Exception as e:
            print(cred, "[TH]:", ser.name, "Error:",creset, e)

            break

##################### Thread #####################
def serthread():

    try:
        ser = serial.Serial(port, speed, timeout=0.1)
        print("[TH]: start ",ser.name)
    except Exception as e:
        print(cred, "[TH] port open error: ",creset, str(e))
        return

    while True:
        time.sleep(5)
        pasket = bytes([ head, comm, data1, data2, data3, data4, data5, data6, crc])
        ser.flush()
        send_bin(ser, pasket)
        dat = read_data(ser, timeout=3)
        if dat == None:
            continue
        if dat == False:
            continue
        ppm = (uint8(dat[1]) << 8) + uint8(dat[2])

        def_co2 = 450
        rec_co2 = def_co2 + 350
        norm_co2 = def_co2 + 700
        alert1 = 1500
        alert2 = 2500
        alert3 = 4900

        col = Fore.BLUE
        if ppm > def_co2:
            col = Fore.GREEN

        if ppm > rec_co2:
            col = Fore.LIGHTGREEN_EX
        if ppm > norm_co2:
            col = Fore.YELLOW
        if ppm > alert1:
            col = Fore.LIGHTYELLOW_EX
        if ppm > alert2:
            col = Fore.LIGHTRED_EX
        if ppm > alert3:
            col = Fore.RED

        prs = int(50/5000 * ppm)
        bar = "▓" * prs + "-" * (50 - prs)
        print(f'{col}[{bar}] {ppm}')

        #log
        # f = open('log.txt', 'a')
        # f.write(f'{time.time()};{ppm}\n')
        # f.close()

        

    

    

##################### MAIN #####################
if __name__ == '__main__':
    
    print(creset, "[main]: start")
    print(creset, "[main]: open threads:")


    serial_thread1 = Thread(target=serthread, args=(), daemon=True) 
    serial_thread1.start()
    
    print(creset, "[main]: while...")
    while True:
        time.sleep(10)
        if serial_thread1.is_alive() == False:
            print(cyellow, "[main]: TH restart", creset)
            port = find_port()[0]
            serial_thread1 = Thread(target=serthread, args=(), daemon=True) 
            serial_thread1.start()
            
   