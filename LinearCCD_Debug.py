import json
import time
import serial
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

x = range(128)
dataArray = x
figure, ax = plt.subplots()
lineFigure = ax.plot(x, dataArray)

try:
    com = serial.Serial('/dev/ttyACM0', 115200, timeout=50)
    if com.isOpen() == True:
        print('Serial Open Successful')
except Exception as e:
    print('Unable to open Serial Port: ', e)
    exit()

def UpdateFigure(i):
    bufferSize = com.in_waiting
    if bufferSize:
        buffer = com.read_all()
        lineList = buffer.decode('utf-8').split('\n')
        for line in lineList:
            try:
                leftIndex = line.index('{')
                rightIndex = line.index('}')
                jsonStr = line[leftIndex:rightIndex+1]
                print(jsonStr)
                loadDict = json.loads(jsonStr)
                dataArray = loadDict['Pixel']
                ax.cla()
                ax.plot(x, dataArray)
                mean = np.mean(dataArray)
                median = np.median(dataArray)
                ax.hlines(mean, x[0], x[127], color='g', label='Average')
                ax.hlines(median, x[0], x[127], color='r', label='Median')
            except Exception as e:
                pass

anim = animation.FuncAnimation(figure, UpdateFigure, 200)
plt.show()
