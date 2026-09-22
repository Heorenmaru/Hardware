import serial #pyserial
import serial.tools.list_ports
import time
from threading import Thread
from numpy import int32, uint32, uint8, int8, uint64

import os
import sys
import traceback
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
print("[pre_main]: go to file directory")
#go to file directory
try:
    os.chdir(os.path.dirname(sys.argv[0]))
except:
    pass

dev_vid_pid = '0483:5740'
def setVidPid(vstr):
    global dev_vid_pid
    dev_vid_pid = vstr

def find_port(vid_pid: str = '0483:5740'):
    """
    :param vid_pid: VID and PID of the device (for stm32 it's '0483:5740')
    :return: port name in OS
    """

    device = None
    devices = []
    ports = serial.tools.list_ports.comports()
    for port in serial.tools.list_ports.comports():
        if vid_pid in port.hwid:
            device = port.device
        if device is not None:
            devices.append(device.__str__())
    return devices


def check_port(port):
    try:
        ser = serial.Serial(port)
        ser.close()
        return True
    
    except:
        return False

def find_free_port(vid_pid: str = '0483:5740'):
    ports = find_port(vid_pid)
    for port in ports:
        if check_port(port):
            return port
    return ''

#=================================================================

port = ''  # само найдет
speed = 115200  # для USB не имеет значения

#=================================================================



#=================================================================
#  RX CALBACK  
#  handling of the received data
#=================================================================


ser = 0

def _rx_calback(data):
    ##########################################################
    #   data  -  array []
    ##########################################################
    print(cgreen, data)   

    


   




#=================================================================



def send(data):
    global ser
    try:

        packet = bytes([0xAA, len(data)])
        packet = packet + bytes(data)
        #print(cblue,'send -> ', [f'{byte:02X}'for byte in list(packet)])
        ser.write(packet)
    except Exception as e:
        print(cred,'SERIAL WRITE ERROR!')
        print(e)
        return False
    pass
    return True






ext_trg = 0
##################### Thread #####################
def serthread(callback_fn, port = find_free_port()):
    global ext_trg
    global ser

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
        return

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
                if ext_trg == 1:
                    break
        except Exception as e:
            print(cred, "[STH] ERROR inWaiting:", creset, ser.name,"Error:",e)
            print(cred, "[STH] Port data: ", creset, ser)
            break
        if ext_trg == 1:
            print(cblue, "[STH] exit", creset, ser)
            break
        ###################################
        # read data
        ###################################
        
        time.sleep(0.005)
        try:
            ser_wait = ser.inWaiting()
        except:
            print(cred, "[STH] ser.inWaiting() ", creset, ser)
            break
        while ser_wait > 0 :
                
            try:
                ser_wait = ser.inWaiting()
            except:
                print(cred, "[STH] ser.inWaiting() ", creset, ser)
                break
        

            data_byte = int.from_bytes(ser.read(size=1), "big")
            #print(data_byte)
            
            try:
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
            except Exception as e:
                print(cred, "[TH]:", ser.name, "Error:",creset, e)
                tb_str = traceback.format_exc()
                print(tb_str)
                break       
            ###################################
            #callback
            ###################################
            if packet_step == 5:
                #print(cblue, "[TH]: ",rec_arr)   ########## debug data
                callback_fn(rec_arr)
                packet_step = 0
                packet_len  = 0
                packet_count = 0
                rec_arr = []
            


        #except Exception as e:
        #    print(cred, "[TH]:", ser.name, "Error:",creset, e)
        #    tb_str = traceback.format_exc()
        #    print(tb_str)
        #    break
    
def StartListener(callback_fn = _rx_calback, port = find_free_port(dev_vid_pid), autofind = True, autorestart = True):
    global ext_trg
    sport = port
    ext_trg = 0
    
    print(creset, "[logger_main]: start")
    print(creset, "[logger_main]: open threads:")


    serial_thread = Thread(target=serthread, args=(callback_fn,sport,), daemon=True) 
    serial_thread.start()
    
    
    print(creset, "[logger_main]: while...")
    while True:
        time.sleep(1)
        if autorestart == False:
            break
        

        if serial_thread.is_alive() == False:
            if autofind == True:
                sport = find_free_port(dev_vid_pid)
            print(cblue, "use setVidPid('') to set device VID and PID, from autosearch port")
            print(cblue, "example: setVidPid('0483:5740') #stm32 virtual com-port")
            if ext_trg == 1:
                break
            print(creset, "[logger_main]: TH restart")
            print(cyellow, "[logger_main]: TH restart", creset)
            serial_thread = Thread(target=serthread, args=(callback_fn,sport,), daemon=True) 
            serial_thread.start()
    print(cblue, "[logger_main] exit", creset)            

def StartListener_BG(callback_fn = _rx_calback, port = find_free_port(dev_vid_pid), autofind = True):
    listener = Thread(target=StartListener, args=(callback_fn, port, autofind, ), daemon=True) 
    listener.start()

def StopLogger():
    global ext_trg
    global ser
    ext_trg = 1
    ser = 0
    

##################### MAIN #####################
if __name__ == '__main__':
    StartListener()
