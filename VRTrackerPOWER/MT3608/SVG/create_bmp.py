import xml.etree.ElementTree as ET
import re
from PIL import Image, ImageDraw, ImageFont
import json
import os

def createBMP(svg_file):

    if not(os.path.exists(svg_file)):
        print(f'FAIL! NOT FOUND:  {svg_file}')

    # 1620x2560
    # 505 ppi

    def mm_to_px(mm, ppi):
        """
        Преобразует координаты из миллиметров в пиксели.

        :param mm: Значение в миллиметрах.
        :param ppi: Плотность пикселей на дюйм (PPI).
        :return: Значение в пикселях.
        """
        px = mm * (ppi / 25.4)
        return round(px)

    def parse_svg_to_tree(element):
        node = {
            'tag': element.tag.split('}')[-1],  # Удаление namespace
            'attrs': element.attrib,
            'children': []
        }
        for child in element:
            node['children'].append(parse_svg_to_tree(child))
        return node

    # Регистрация SVG namespace (важно для корректного парсинга)
    ET.register_namespace('', "http://www.w3.org/2000/svg")

    # Парсинг файла
    tree = ET.parse(svg_file)
    root = tree.getroot()

    # Преобразование в древовидную структуру
    svg_elems = [parse_svg_to_tree(root)]



    cmds = []

    cw = (0,0,0)
    cb = (255,255,255)

    if "-F_Mask" in svg_file:
        cw = (255,255,255)
        cb = (0,0,0)

    imgWidth = 2560
    imgHeight = 1620
    ppi = 505


    minx = imgWidth
    miny = imgHeight
    maxx = 0
    maxy = 0

    bwidth = svg_elems[0]['attrs']['width']
    bheight = svg_elems[0]['attrs']['height']
    for elem in svg_elems[0]['children']:
        if elem['tag'] == 'title':
            continue
        if elem['tag'] == 'desc':
            continue
        if elem['tag'] == 'g':
            color = cb
            if 'fill:none' in elem['attrs']['style']:
                color = cw
            pwidth = 0
            try:
                st = elem['attrs']['style'].split(';')
                for sta in st:
                    staa = sta.split(':')
                    if 'stroke-width' in staa[0]:
                        pwidth = float(staa[1])
            except:
                pass
            
            cmds.append({'cmd': 'pen', 'color':color, 'width': pwidth})
            if len(elem['children'])>0:
                for el in elem['children']:
                    ##### RECT
                    if el['tag'] == 'rect':
                        x1 = float(el['attrs']['x'])
                        y1 = float(el['attrs']['y'])
                        x2 = x1 + float(el['attrs']['width'])
                        y2 = y1 + float(el['attrs']['height'])
                        cmds.append({'cmd': 'rect', 
                                     'x1':x1, 
                                     'y1':y1, 
                                     'x2':x2,
                                     'y2':y2})
                        # if minx > x1:
                        #     minx = x1                    
                        # if miny > y1:
                        #     miny = y1
                        # if maxx < x2:
                        #     maxx = x2
                        # if maxy < y2:
                        #     maxy = y2
                        continue
                    if el['tag'] == 'path':
                        s = el['attrs']['d']
                        ##### Line
                        if 'L' in s:
                            s = s.split(' ')
                            mv = ( float( s[0].replace('M','')), float(s[1]) )
                            to = ( float( s[2].replace('L','')), float(s[3])  )
                            coord = [mv,to]
                            cmds.append({'cmd': 'line', 'coord':coord})
                            continue
                        ##### POLIGON
                        s = s.split(' ')
                        pnts = []

                        for p in s:
                            if p == 'M':
                                continue
                            if p == 'Z':
                                continue
                            xy = p.split(',')
                            px = float(xy[0])
                            py = float(xy[1])

                            if minx > px:
                                minx = px                    
                            if miny > py:
                                miny = py
                            if maxx < px:
                                maxx = px
                            if maxy < py:
                                maxy = py
                            pnts.append((px,py))
                        cmds.append({'cmd': 'polygon', 'points':pnts})
                        continue

                    if el['tag'] == 'circle':
                        cmds.append({'cmd': 'circle', 
                                     'x':float(el['attrs']['cx']), 
                                     'y':float(el['attrs']['cy']), 
                                     'r':float(el['attrs']['r'])
                                     })
                        continue
                    print(f"Я ХЗ ЧЕ ЭТО: {el}")
                    print(f"ДОПИЛИВАЙ МЕНЯ!!!")
                    input()
            continue
        


    ################## DRAW



    img = Image.new('RGB', (imgWidth, imgHeight), color=cw)

    # Создание объекта для рисования
    draw = ImageDraw.Draw(img)

    color = 'black'
    pwidth = 3

    xoff = int(imgWidth/2 - (mm_to_px(maxx, ppi) + mm_to_px(minx, ppi))/2)
    yoff = int(imgHeight/2 - (mm_to_px(maxy, ppi) + mm_to_px(miny, ppi))/2)

    try:
        f = open('offset.json', 'r')
        offsets = json.load(f)
        f.close()
        xoff = offsets['x']
        yoff = offsets['y']
    except FileNotFoundError:
        xoff = int(imgWidth/2 - (mm_to_px(maxx, ppi) + mm_to_px(minx, ppi))/2)
        yoff = int(imgHeight/2 - (mm_to_px(maxy, ppi) + mm_to_px(miny, ppi))/2)
        offsets = {'x': xoff, 'y': yoff}
        f = open('offset.json', 'w')
        json.dump(offsets, f)
        f.close()


    # xoff = -1700
    # yoff = -1200
    for c in cmds:
        if c['cmd'] == 'pen':
            color = c['color']
            pwidth = mm_to_px(c['width'], ppi) 
            continue

        if c['cmd'] == 'rect':
            x1 = mm_to_px(c['x1'], ppi) + xoff
            y1 = mm_to_px(c['y1'], ppi) + yoff
            x2 = mm_to_px(c['y2'], ppi) + xoff
            y2 = mm_to_px(c['y2'], ppi) + yoff
            draw.rectangle([x1, y1, x2, y2], fill = cb)
            continue

        if c['cmd'] == 'polygon':
            pn = []
            for p in c['points']:
                pn.append( ( mm_to_px(p[0], ppi) + xoff, mm_to_px(p[1], ppi) + yoff ) )
            draw.polygon(pn, fill = cb)    
            continue

        if c['cmd'] == 'circle':
            draw.circle([mm_to_px(c['x'], ppi) + xoff,  mm_to_px(c['y'], ppi) + yoff ], mm_to_px(c['r'], ppi), fill=cb)    
            continue

        if c['cmd'] == 'line':
            x1 = mm_to_px(c['coord'][0][0], ppi) + xoff
            y1 = mm_to_px(c['coord'][0][1], ppi) + yoff
            x2 = mm_to_px(c['coord'][1][0], ppi) + xoff
            y2 = mm_to_px(c['coord'][1][1], ppi) + yoff

            draw.circle( (x1, y1), (pwidth // 2) -1, fill = cb)
            draw.circle( (x2, y2), (pwidth // 2) -1, fill = cb)
            draw.line([x1,y1,x2,y2], fill= cb, width = (pwidth -1), joint='curve')    
            continue

        print(f"Я ХЗ ЧЕ ЭТО: {c['cmd']}")
        print(f"в парсер то добавил, а в Draw забыл!!")
        input()

    pixels1 = img.load()
    # Отображение изображения
    # img.show()





    # Packing img


    def parse_image(img):
        # Получаем размеры исходного изображения
        width, height = img.size

        # Вычисляем высоту нового изображения
        new_height = height // 3
        img2 = Image.new('RGB', (width, new_height))
        pixels2 = img2.load()

        # Загружаем пиксели исходного изображения
        pixels = img.load()

        for y in range(new_height):
            # Вычисляем номера строк для каждого канала
            red_line = y * 3
            green_line = y * 3 + 1
            blue_line = y * 3 + 2

            for x in range(width):
                # Извлекаем компоненты из соответствующих строк
                r = pixels[x, red_line][0]
                g = pixels[x, green_line][1]
                b = pixels[x, blue_line][2]

                # Собираем новый пиксель
                pixels2[x, y] = (r, g, b)

        return img2






    # Пример использования
    mirror_res = img.transpose(Image.FLIP_LEFT_RIGHT)

    # линейки
    # draw2 = ImageDraw.Draw(mirror_res)

    # wi = mm_to_px(maxx, ppi) - mm_to_px(minx, ppi)
    # he = mm_to_px(maxy, ppi) - mm_to_px(miny, ppi)
    # draw2.line([xoff-15, yoff-15, xoff+ wi, yoff-15], fill= cb, width = 1, joint='curve')
    # draw2.line([xoff-15, yoff-15,     xoff, yoff+ he], fill= cb, width = 1, joint='curve')

    result_img = parse_image(mirror_res)
    
    # mirror_res.show()

    result_img.save(f'{svg_file}.bmp')



def sorted_svg_files():
    all_files = os.listdir('.')
    svg_files = [f for f in all_files if f.endswith('.svg')]
    
    # Сортировка с приоритетом для файлов с "-F_Cu"
    return sorted(svg_files, key=lambda x: ('-F_Cu' not in x, x))

svgfiles = sorted_svg_files()
for fle in svgfiles:
    createBMP(fle)


