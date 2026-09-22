import USBloggerLib
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
       
        print(Fore.LIGHTRED_EX,  f'{data}', )

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

def sensors2(data):
    try:
       
        print(Fore.LIGHTGREEN_EX,  f'{data}', )

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

def sensors3(data):
    try:
       
        print(Fore.LIGHTBLUE_EX,  f'{data}', )

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

tasks = {"255":sensors,
         "254":sensors2,
         "253":sensors3}

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
    USBloggerLib.send([0x00,0b00000001])
    tasks['0']=checkDevice(data, stp=1)
    

def startADC():
    cmd = 0x01

    start_time = time.time()
    while (start_time + 10) > time.time():
        if USBloggerLib.send([cmd]):
            break
        else:
            time.sleep(0.5)



############################################################################
### input type: data = [1,2,3,...]
############################################################################
if __name__ == "__main__":



    USBloggerLib.StartListener_BG(callback_fn=usb_rx)
    


    while 1:
        time.sleep(0.010)
        #startADC()
