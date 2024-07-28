import USBLib
import time
from datetime import datetime ,timezone, timedelta
from numpy import uint32, uint8, int8, uint16
import numpy as np
import struct

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

import os
import sys
try:
    #go to file directory
    os.chdir(os.path.dirname(sys.argv[0]))
except:
    pass
filename = 'log.csv'

#define ADC_REFERENCE_VOLTAGE					1.224f
#define ADC_MAX									0x1000//0xFFF

############################################################################
def sensors(data):
    try:
        C = data[0] | data[1]<<8
        R = data[2] | data[3]<<8
        G = data[4] | data[5]<<8
        B = data[6] | data[7]<<8

        #adc4 = adc4 * ( (RV_HI4 + RV_LO4) / RV_LO4 )
        #adc5 = adc5 * ( (RV_HI5 + RV_LO5) / RV_LO5 )
        #adc6 = adc6 * ( (RV_HI6 + RV_LO6) / RV_LO6 )
        #adc7 = adc7 * ( (RV_HI7 + RV_LO7) / RV_LO7 )

        print(cgreen, f'C: {C:7d}, '+
                      f'R: {R:7d}, '+
                      f'G: {G:6d}, '+    
                      f'B: {B:6d}, '
                      )

        #if not os.path.exists(filename):
        #    file =  open(filename, mode='w') 
        #    file.write('Time;Vref;Adc0;Adc1;Adc2;Adc3;Adc4;Adc5;Adc6;Adc7\n')
        #    file.close()


        #now = datetime.now(timezone.utc)
        #formatted_time = now.strftime('%Y-%m-%d %H:%M:%S')
        #file =  open(filename, mode='a') 
        #file.write(f'{formatted_time};{vref:.5f};{adc0:.5f};{adc1:.5f};{adc2:.5f};{adc3:.5f};{adc4:.5f};{adc5:.5f};{adc6:.5f};{adc7:.5f}\n')
        #file.close()

    except Exception as e:  
        print(e)

in1_max = 0
in1_min = 0
in2_max = 0
in2_min = 0
in3_max = 0
in3_min = 0


tasks = {"255":sensors}

def usb_rx(data):
    global receivedTrg
    receivedTrg = 1
    try:
        tasks[str(data[0])](data[1:])
    except:
        pass
        #print(cgreen, data)   


############################################################################

def checkDevice(data, stp = 0):
    USBLib.send([0x00,0b00000001])
    tasks['0']=checkDevice(data, stp=1)
    

def startADC():
    cmd = 0x01

    start_time = time.time()
    while (start_time + 10) > time.time():
        if USBLib.send([cmd]):
            break
        else:
            time.sleep(0.5)

def startADC_cont():
    cmd = 0x02

    start_time = time.time()
    while (start_time + 10) > time.time():
        if USBLib.send([cmd]):
            break
        else:
            time.sleep(0.5)

def ADC_int(d): # 1/0
    cmd = 0x03
    dat = d
    start_time = time.time()
    while (start_time + 10) > time.time():
        if USBLib.send([cmd,dat]):
            break
        else:
            time.sleep(0.5)


############################################################################
### input type: data = [1,2,3,...]
############################################################################
if __name__ == "__main__":



    USBLib.StartListener_BG(callback_fn=usb_rx)
    


    while 1:
        time.sleep(0.010)
        #startADC()
