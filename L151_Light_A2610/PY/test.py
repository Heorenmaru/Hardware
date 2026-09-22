import random
import threading
import time
import tkinter as tk
from PIL import Image, ImageTk
import USBloggerLib
from numpy import uint32, uint8, int8, uint16
import numpy as np
import os
import sys
try:
    #go to file directory
    os.chdir(os.path.dirname(sys.argv[0]))
except:
    pass
# Настройки размеров
WIDTH, HEIGHT = 300, 300
AREA_SIZE = 18


rf = []
gf = []
bf = []
# Создаем базовое изображение (черное)
img = Image.new("RGB", (WIDTH, HEIGHT), color=(0, 0, 0))
x = 0
y = 0


def draw_red(x, y, val):
    """Изменяет только Красный (R) канал в области 18x18"""
    pixels = img.load()
    r, g, b = pixels[x, y]
    pixels[x,y] = (val*4, g, b)
    # request_update()

def draw_green(x, y, val):
    """Изменяет только Зеленый (G) канал в области 18x18"""
    pixels = img.load()
    r, g, b = pixels[x, y]
    pixels[x,y] = (r, val*4, b)
    # request_update()

def draw_blue(x, y, val):
    """Изменяет только Синий (B) канал в области 18x18"""
    pixels = img.load()
    r, g, b = pixels[x, y]
    pixels[x,y] = (r, g, val*4)
    # request_update()



def request_update():
    """Безопасно запрашивает обновление интерфейса из любого потока"""
    root.after(0, update_canvas)


def update_canvas():
    """Обновляет изображение на холсте (выполняется в главном потоке)"""
    global img_tk
    global pointer_id
    img_tk = ImageTk.PhotoImage(img)
    canvas.create_image(0, 0, anchor=tk.NW, image=img_tk)

    canvas.coords(pointer_id, x , y , x + 18, y + 18 )
    canvas.itemconfig(pointer_id, state=tk.NORMAL)
    canvas.tag_raise(pointer_id)


def background_worker():
    """Функция, которая будет крутиться в отдельном потоке и генерировать рисунки"""
    # Даем окну Tkinter загрузиться
    time.sleep(0.5)
    while 1:
        time.sleep(0.1)
        request_update()


def red_data(data):
    global x
    global y
    
    dx = np.array(data[0]).astype(int8)
    dy = np.array(data[1]).astype(int8)
    pind = int(data[3])<<8 | int(data[2])
    x = x+dx
    y = y+dy

    if x<0: x=0
    if x>(WIDTH-18): x = WIDTH-18

    if y<0: y=0
    if y>(HEIGHT-18): y = HEIGHT-18

    py = int(pind/18)
    px = pind - 18*py
    pv= data[4]
    draw_red(x+px, y+py, pv & 0b00111111)


def green_data(data):
    global x
    global y
    
    dx = np.array(data[0]).astype(int8)
    dy = np.array(data[1]).astype(int8)
    pind = int(data[3])<<8 | int(data[2])
    x = x+dx
    y = y+dy

    if x<0: x=0
    if x>(WIDTH-18): x = WIDTH-18

    if y<0: y=0
    if y>(HEIGHT-18): y = HEIGHT-18

    py = int(pind/18)
    px = pind - 18*py
    pv= data[4]
    draw_green(x+px, y+py, pv & 0b00111111)


def blue_data(data):
    global x
    global y
    
    dx = np.array(data[0]).astype(int8)
    dy = np.array(data[1]).astype(int8)
    pind = int(data[3])<<8 | int(data[2])
    x = x+dx
    y = y+dy

    if x<0: x=0
    if x>(WIDTH-18): x = WIDTH-18

    if y<0: y=0
    if y>(HEIGHT-18): y = HEIGHT-18

    py = int(pind/18)
    px = pind - 18*py
    pv= data[4]
    draw_blue(x+px, y+py, pv & 0b00111111)


tasks = {"255":red_data,
         "254":green_data,
         "253":blue_data}

def usb_rx(data):
    global receivedTrg
    receivedTrg = 1
    try:
        tasks[str(data[0])](data[1:])
    except:
        pass
        #print(cgreen, data)   


# Создание окна Tkinter
root = tk.Tk()
root.title("RGB Threaded Painter")

# Создаем Canvas для отрисовки
canvas = tk.Canvas(root, width=WIDTH, height=HEIGHT)
canvas.pack()

# Инициализируем первое отображение
img_tk = ImageTk.PhotoImage(img)
canvas.create_image(0, 0, anchor=tk.NW, image=img_tk)

pointer_id = canvas.create_rectangle(0, 0, 0, 0, outline="green", width=1, state=tk.HIDDEN)


# # Запуск фонового потока
# # daemon=True означает, что поток автоматически закроется при закрытии главного окна
thread = threading.Thread(target=background_worker, daemon=True)
thread.start()

USBloggerLib.StartListener_BG(callback_fn=usb_rx)

# Запуск главного цикла окна
root.mainloop()
