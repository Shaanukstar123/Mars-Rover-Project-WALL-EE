import numpy
import cv2
path = "C:/Users/Shaheen/Documents/Year2/Wall-EE/Wall-EE/Vision/PythonHelpers/Images/"
while True:
    try:
        imageName = input("Enter image name: ")
        imageAnalyse = cv2.imread(path+imageName)
        hsvImage = cv2.cvtColor(imageAnalyse, cv2.COLOR_BGR2HSV)
        while True:
            lowThresholds = [int(i) for i in input("Enter lower HSV: ").split(' ')]
            highThresholds = [int(i) for i in input("Enter upper HSV: ").split(' ')]
            mask = cv2.inRange(hsvImage, numpy.array(lowThresholds), numpy.array(highThresholds))
            final = cv2.bitwise_and(imageAnalyse, imageAnalyse, mask = mask)
            cv2.imshow('final', final)
            cv2.waitKey(0)
            cv2.destroyAllWindows()
    except Exception as e:
        print(e)