import cv2
import numpy as np

def nothing(x):
    pass

# Load image
path = "C:/Users/Shaheen/Documents/Year2/Wall-EE/Wall-EE/Vision/PythonHelpers/Images/"
while True:
    try:
        imageName = input("Enter image name: ")
        image = cv2.imread(path+imageName)

        # Create a window
        cv2.namedWindow('sliders')
        cv2.namedWindow('image')

        # Create trackbars for color change
        # Hue is from 0-179 for Opencv
        cv2.createTrackbar('HMin', 'sliders', 0, 359, nothing)
        cv2.createTrackbar('SMin', 'sliders', 0, 255, nothing)
        cv2.createTrackbar('VMin', 'sliders', 0, 255, nothing)
        cv2.createTrackbar('HMax', 'sliders', 0, 359, nothing)
        cv2.createTrackbar('SMax', 'sliders', 0, 255, nothing)
        cv2.createTrackbar('VMax', 'sliders', 0, 255, nothing)

        # Set default value for Max HSV trackbars
        cv2.setTrackbarPos('HMax', 'sliders', 359)
        cv2.setTrackbarPos('SMax', 'sliders', 255)
        cv2.setTrackbarPos('VMax', 'sliders', 255)

        # Initialize HSV min/max values
        hMin = sMin = vMin = hMax = sMax = vMax = 0
        phMin = psMin = pvMin = phMax = psMax = pvMax = 0

        while(1):
            # Get current positions of all trackbars
            hMin = cv2.getTrackbarPos('HMin', 'sliders')
            sMin = cv2.getTrackbarPos('SMin', 'sliders')
            vMin = cv2.getTrackbarPos('VMin', 'sliders')
            hMax = cv2.getTrackbarPos('HMax', 'sliders')
            sMax = cv2.getTrackbarPos('SMax', 'sliders')
            vMax = cv2.getTrackbarPos('VMax', 'sliders')

            # Set minimum and maximum HSV values to display
            lower = np.array([hMin/2, sMin, vMin])
            upper = np.array([hMax/2, sMax, vMax])

            # Convert to HSV format and color threshold
            hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
            mask = cv2.inRange(hsv, lower, upper)
            result = cv2.bitwise_and(image, image, mask=mask)

            # Print if there is a change in HSV value
            if((phMin != hMin) | (psMin != sMin) | (pvMin != vMin) | (phMax != hMax) | (psMax != sMax) | (pvMax != vMax) ):
                print("(hMin = %d , sMin = %d, vMin = %d), (hMax = %d , sMax = %d, vMax = %d)" % (hMin , sMin , vMin, hMax, sMax , vMax))
                phMin = hMin
                psMin = sMin
                pvMin = vMin
                phMax = hMax
                psMax = sMax
                pvMax = vMax

            # Display result image
            cv2.imshow('image', result)
            if cv2.waitKey(10) & 0xFF == ord('q'):
                break

        cv2.destroyAllWindows()
    except:
        None