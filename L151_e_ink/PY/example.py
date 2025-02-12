import USBLib
import time
from datetime import datetime ,timezone, timedelta
from numpy import uint32, uint8, int8, uint16, int16
import numpy as np
from PIL import Image


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


############################################################################
def rawdat(data):
    try:
       print(f'raw data: {data}')

    except Exception as e:  
        print(e)



tasks = {"255":rawdat}

def usb_rx(data):
    global receivedTrg
    receivedTrg = 1
    try:
        tasks[str(data[0])](data[1:])
    except:
        pass
        #print(cgreen, data)   


def convert_to_black_and_white(image_path, threshold=128):
    """
    Загружает изображение, преобразует в черно-белый вид и возвращает массив пикселей.

    Args:
        image_path (str): Путь к изображению.
        threshold (int): Порог для определения, является ли пиксель черным или белым (0-255).

    Returns:
        numpy.ndarray: Массив пикселей, где 0 - черный, 255 - белый.
    """
    try:
        # Открываем изображение
        img = Image.open(image_path).convert('L')  # 'L' означает преобразование в оттенки серого

        # Получаем пиксели изображения
        pixels = np.array(img)

        # Преобразуем в черно-белое изображение
        binary_pixels = np.where(pixels < threshold, 0, 255).astype(np.uint8)

        return binary_pixels

    except FileNotFoundError:
        print(f"Ошибка: Файл не найден по пути {image_path}")
        return None
    except Exception as e:
        print(f"Произошла ошибка: {e}")
        return None

# Пример использования

    

############################################################################
def respCheckDevice(data):

    try:
        uid = [data[0], data[1], 
            data[2], data[3], 
            data[4], data[5], data[6], data[7], 
            data[8], data[9], data[10], data[11]]
        dev_code = data[12]<<8 | data[13]
        dev_ver = data[14]

        print(f'{cgreen} uid: {uid}, dev_code: {dev_code}, dev_ver: {dev_ver}')
    except:
        print('ERROR DEV CHECK')
def checkDevice():
    start_time = time.time()
    cmd = 0x00 
    dat = [cmd] 
    tasks['0'] = respCheckDevice
    while (start_time + 10) > time.time():
        if USBLib.send(dat):
            break
        else:
            time.sleep(0.0001)
            print("px_retry")
    # USBLib.send([0x00])
    
    

# write img
sendStatus = 0
def sendStatusRX(data):
    global sendStatus
    sendStatus = data[0]

def sendimg(arr):
    global sendStatus
    cmd = 0x01  

    n = int16(0)
    cnt= uint8(32)

    img_data = []


    
    for y in range(19):
        for x in range(151):
            ibyte = 0
            try:
                if arr[y*8  ][x] > 128: ibyte = ibyte | 1<<7
                if arr[y*8+1][x] > 128: ibyte = ibyte | 1<<6
                if arr[y*8+2][x] > 128: ibyte = ibyte | 1<<5
                if arr[y*8+3][x] > 128: ibyte = ibyte | 1<<4
                if arr[y*8+4][x] > 128: ibyte = ibyte | 1<<3
                if arr[y*8+5][x] > 128: ibyte = ibyte | 1<<2
                if arr[y*8+6][x] > 128: ibyte = ibyte | 1<<1
                if arr[y*8+8][x] > 128: ibyte = ibyte | 1<<0
            except:
                pass
            img_data.append(ibyte)



    # send part

    while n< len(img_data):
        dat = [cmd] 
        dat.append(n>>8)
        dat.append(n&0xFF)
        dat.append(cnt)
        cnt = 0
        for i in range(32):
            try:
                dat.append(img_data[n+cnt])
                cnt += 1
            except:
                pass
        dat[3]= cnt
        
        start_time = time.time()
        
        sendStatus = 0
        tasks['1'] = sendStatusRX
        while (start_time + 10) > time.time():
            if USBLib.send(dat):
                break
            else:
                time.sleep(0.0001)
                print("snd img part retry")
        start_time = time.time()
        
        while sendStatus == 0 :
            if (start_time + 10) < time.time():
                print("snd img ERROR!!")
                return
        n = n + cnt 
    
    # update display
    print("Update...")
    cmd = 0x02
    dat = [cmd] 
    while (start_time + 10) > time.time():
        if USBLib.send(dat):
            break
        else:
            time.sleep(0.0001)
            print("UPD img retry")


    
    # start_time = time.time()
    # while (start_time + 10) > time.time():
    #     if USBLib.send(dat):
    #         break
    #     else:
    #         time.sleep(0.0001)
    #         print("px_retry")



############################################################################
### input type: data = [1,2,3,...]
############################################################################
if __name__ == "__main__":




    USBLib.StartListener_BG(callback_fn=usb_rx)
    
    time.sleep(2)

    checkDevice()
    image_path = '30.png'  # Замените на путь к вашему изображению
    black_and_white_array = convert_to_black_and_white(image_path,threshold=200)

    if black_and_white_array is not None:
        print("Sending img...")
        sendimg(black_and_white_array)
    
    print("OK")
    # while 1:

    #     time.sleep(5)

