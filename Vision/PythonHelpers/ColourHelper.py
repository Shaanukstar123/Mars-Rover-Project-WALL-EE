import tkinter
import colorsys
import math
root = tkinter.Tk()
canvas = tkinter.Canvas(root, width=1280, height=720)
HueSep = 16
SatSep = 4
ValSep = 2
pixelSize = 16
for i in range(ValSep):
    (r, g, b) = colorsys.hsv_to_rgb(0, 0, (i/ValSep))
    hexStr = '#%02x%02x%02x' % (int(r*255), int(g*255), int(b*255))
    canvas.create_rectangle(pixelSize*i, 0,  pixelSize*(i+1), pixelSize, fill=hexStr)
l = -1
for i in range(math.ceil(math.sqrt(HueSep))):
    for i2 in range(math.ceil(math.sqrt(HueSep))):
        l += 1
        for j in range(SatSep):
            for k in range(1, ValSep+1):
                (r, g, b) = colorsys.hsv_to_rgb((l/HueSep), (j/SatSep), (k/ValSep))
                hexStr = '#%02x%02x%02x' % (int(r*255), int(g*255), int(b*255))
                canvas.create_rectangle((pixelSize*j)+i2*(pixelSize*(SatSep)), (pixelSize*k)+i*(pixelSize*(ValSep)), (pixelSize*(j+1))+(i2)*((pixelSize)*(SatSep)), (pixelSize*(k+1)+(i)*(pixelSize*(ValSep))), fill=hexStr)
for i in range(HueSep):
    (r, g, b) = colorsys.hsv_to_rgb((i/HueSep), 1, 1)
    hexStr = '#%02x%02x%02x' % (int(r*255), int(g*255), int(b*255))
    canvas.create_rectangle((pixelSize*i), ValSep*pixelSize*(1+math.ceil(math.sqrt(HueSep))), pixelSize*(i+1), pixelSize+ValSep*pixelSize*(1+math.ceil(math.sqrt(HueSep))), fill=hexStr)
canvas.pack()
root.mainloop()