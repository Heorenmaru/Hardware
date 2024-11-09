import USBLib
import time
from datetime import datetime ,timezone, timedelta
from numpy import uint32,int32, uint8, int8, uint16, int16
import numpy as np
import struct

np.seterr(all="ignore", over="ignore")
uint8 = np.uint8
#FUCKING WARNINGS!!!

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
        dat = data[1] | data[2]<<8 | data[3]<<16
        if dat> (1<<23):
            dat = dat - (1<<24)


        print(cgreen, hex(dat),dat)



    except Exception as e:  
        print(e)



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
### input type: data = [1,2,3,...]
############################################################################
if __name__ == "__main__":




    USBLib.StartListener_BG(callback_fn=usb_rx)
    
    time.sleep(2)

    while 1:

    
        
        time.sleep(5)

