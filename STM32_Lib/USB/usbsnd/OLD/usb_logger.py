import serial #pyserial
import serial.tools.list_ports
import time
from datetime import datetime
from threading import Thread
from numpy import int32, uint32, uint8, int8, uint64


def find_port(vid_pid: str = '0483:5740') -> str:
    """
    :param vid_pid: VID and PID of the device (for stm32 it's '0483:5740')
    :return: port name in OS
    """

    stm32 = None
    for port in serial.tools.list_ports.comports():
        if vid_pid in port.hwid:
            stm32 = port.device
    if stm32 is not None:
        return stm32.__str__()
    return ''





#=================================================================

port = 'COM15'  # само найдет
speed = 115200  # для USB не имеет значения

#=================================================================



#=================================================================
#  RX CALBACK  
#  handling of the received data
#=================================================================

last_time = 0
count = 0

def rx_calback(data):
    ##########################################################
    #   data  -  array []
    ##########################################################
    global last_time
    global count
    if time.time() - last_time > 1:
        print('--------------------------------------------------')
        count = 0
    last_time = time.time()

    if data[0] == 0x00:
        temp1 =  struct.unpack('i', bytearray([data[1],data[2],data[3],data[4]]) )[0] 
        press1 = struct.unpack('i', bytearray([data[5],data[6],data[7],data[8]]) )[0] 

        temp2 =  struct.unpack('i', bytearray([data[9],data[10],data[11],data[12]]) )[0] 
        press2 = struct.unpack('i', bytearray([data[13],data[14],data[15],data[16]]) )[0] 
        #print(cblue, "[RXCB]: ",clblue, data)

        
        print(clcyan,   "P1: {:>13.3f} Pa   T1: {:>13.3f} °C    ||    P2: {:>13.3f} Pa   T2: {:>13.3f} °C".format(press1, temp1, press2, temp2) )    
    
    if data[0] == 0x01:
        reg =  data[1]
        data = data[2]

        print(clcyan,   "reg: {:3X} data: {:3X} ".format(reg, data) )    
    
    count = count+1







#=================================================================




import struct
# преобразует элементы массива в Float
def decode_float(dt):
    return struct.unpack('>f', bytearray([(dt[0] & 0xFF), (dt[1] >>8), (dt[0] & 0xFF), (dt[0] >>8)]))[0]


import os
import sys
import colorama
from colorama import Fore, Back, Style
colorama.init()
cred =      Fore.RED
cgreen =    Fore.GREEN
clgreen =   Fore.LIGHTGREEN_EX
cblue =     Fore.BLUE
cyellow =   Fore.YELLOW
clyellow =  Fore.LIGHTYELLOW_EX
clcyan =    Fore.LIGHTCYAN_EX
clmag =     Fore.LIGHTMAGENTA_EX
clblue =    Fore.LIGHTBLUE_EX
creset =    Fore.RESET


print("[pre_main]: start")
#go to file directory
try:
    os.chdir(os.path.dirname(sys.argv[0]))
except:
    pass



import json



Th_status = [0,0]
ext_trg = 0
##################### Thread #####################
def serthread():
    cmd_snd = 0
    global Th_status
    global ext_trg
    global port

    rec_arr = []
    packet_step = 0
    packet_len  = 0
    packet_count = 0

    #############################################
    #   Serial Open
    #############################################
    try:
        ser = serial.Serial(port, speed, timeout=0.1)
    except Exception as e:
        print(cred, "[TH] port open error: ",creset, str(e))
        Th_status[1] = 9


        port = find_port()

        return


    Th_status[0] = ser.name
    Th_status[1] = 1

    print(clcyan, "[TH ",ser.name,"]:","Started")


    #############################################
    #   Serial data read
    #############################################
    while True :
        ###################################
        # wait data
        ###################################
        try:
            #wite_trg = 0
            while ser.inWaiting() <=0 :
                time.sleep(0.005)
        except Exception as e:
            print(cred, "[TH] ERROR inWaiting:", creset, ser.name,"Error:",e)
            print(cred, "[TH] Port data: ", creset, ser)
            Th_status[0] = ser.name
            Th_status[1] = 9
            break

        ###################################
        # read data
        ###################################
        try:
            time.sleep(0.005)
            
            while ser.inWaiting() > 0 :
                
                
                data_byte = int.from_bytes(ser.read(size=1), "big")
                #print(data_byte)
                #preamp
                if packet_step == 0:
                    if data_byte == 0xAA:
                        packet_step = 1 
                        #rec_arr.append(data_byte)
                        packet_count = packet_count+1
                        continue
                    else:
                        packet_step = 0
                        packet_len  = 0
                        packet_count = 0
                        rec_arr = []
                #len
                if packet_step == 1:
                    packet_len = data_byte << 8 
                    packet_count = packet_count+1
                    packet_step = 2 
                    
                    continue
                if packet_step == 2:
                    packet_len = packet_len + data_byte & 0xFF
                    packet_count = packet_count+1
                    packet_step = 3
                    continue
                #data
                if packet_step == 3:
                    if packet_count < packet_len:
                        rec_arr.append(data_byte)
                        packet_count = packet_count+1
                        continue
                    else:
                        packet_step = 5
                    
                #crc usb check
                #if packet_step == 4:
                #    packet_crc = data_byte
                #    packet_step = 0
                #    continue
                
                ###################################
                #callback
                ###################################
                if packet_step == 5:
                    #print(cblue, "[TH]: ",rec_arr)   ########## debug data
                    rx_calback(rec_arr)
                    packet_step = 0
                    packet_len  = 0
                    packet_count = 0
                    rec_arr = []
            


        except Exception as e:
            print(cred, "[TH]:", ser.name, "Error:",creset, e)
            Th_status[0] = ser.name
            Th_status[1] = 9
            break
    

##################### MAIN #####################
if __name__ == '__main__':
    
   

    print(creset, "[main]: start")
    print(creset, "[main]: open threads:")


    serial_thread1 = Thread(target=serthread, args=(), daemon=True) 
    serial_thread1.start()
    
    print(creset, "[main]: while...")
    while True:
        time.sleep(1)
        if ext_trg == 1:
            
            exit()
        if (Th_status[1] == 9) or (Th_status[1] == 0):
            print(creset, "[main]: TH restart")
            print(cyellow, "[main]: TH restart", creset)
            serial_thread1 = Thread(target=serthread, args=(), daemon=True) 
            serial_thread1.start()
            
   