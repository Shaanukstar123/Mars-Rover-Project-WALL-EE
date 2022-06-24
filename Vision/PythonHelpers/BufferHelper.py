from random import randint
x = 0
hueDataBuffer = [0]*5
hueStack = [0]*5
saturationDataBuffer = [0]*5
saturationStack = [0]*5
valueDataBuffer = [0]*5
valueStack = [0]*5
while True:
    hueReplaced = 0
    satReplaced = 0
    valReplaced = 0
    finalHue = randint(0, 360)
    finalSat = randint(0, 255)
    finalVal = randint(0, 255)
    tempRegHue = 0
    tempRegSat = 0
    tempRegVal = 0
    for i in range(5):
        if ((not hueReplaced) and (hueDataBuffer[i] == hueStack[x % 5])):
            hueDataBuffer[i] = finalHue 
            hueReplaced = 1 
        if ((not satReplaced) and (saturationDataBuffer[i] == saturationStack[x % 5])):
            saturationDataBuffer[i] = finalSat 
            satReplaced = 1 
        if ((not valReplaced) and (valueDataBuffer[i] == valueStack[x % 5])):
            valueDataBuffer[i] = finalVal 
            valReplaced = 1 
    hueStack[x % 5] = finalHue 
    saturationStack[x % 5] = finalSat 
    valueStack[x % 5] = finalVal
    # Bubble sort
    for i in range(4):
        if (hueDataBuffer[i] > hueDataBuffer[i+1]):
            tempRegHue = hueDataBuffer[i] 
            hueDataBuffer[i] = hueDataBuffer[i+1] 
            hueDataBuffer[i+1] = tempRegHue 
        if (saturationDataBuffer[i] > saturationDataBuffer[i+1]):
            tempRegSat = saturationDataBuffer[i] 
            saturationDataBuffer[i] = saturationDataBuffer[i+1] 
            saturationDataBuffer[i+1] = tempRegSat 
        if (valueDataBuffer[i] > valueDataBuffer[i+1]):
            tempRegVal = valueDataBuffer[i] 
            valueDataBuffer[i] = valueDataBuffer[i+1] 
            valueDataBuffer[i+1] = tempRegVal 
         # Bubble sort
    for i in range(4, 0, -1):
        if (hueDataBuffer[i] < hueDataBuffer[i-1]):
            tempRegHue = hueDataBuffer[i] 
            hueDataBuffer[i] = hueDataBuffer[i-1] 
            hueDataBuffer[i-1] = tempRegHue 
        if (saturationDataBuffer[i] < saturationDataBuffer[i-1]):
            tempRegSat = saturationDataBuffer[i] 
            saturationDataBuffer[i] = saturationDataBuffer[i-1] 
            saturationDataBuffer[i-1] = tempRegSat 
        if (valueDataBuffer[i] < valueDataBuffer[i-1]):
            tempRegVal = valueDataBuffer[i] 
            valueDataBuffer[i] = valueDataBuffer[i-1] 
            valueDataBuffer[i-1] = tempRegVal 
        compHue = hueDataBuffer[2] 
        compSat = saturationDataBuffer[2] 
        compVal = valueDataBuffer[2] 
    x += 1
    if (x % 500)==0:
        None