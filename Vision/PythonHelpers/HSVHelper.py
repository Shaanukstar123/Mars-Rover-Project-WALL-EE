from random import randint
from colorsys import rgb_to_hsv

def accRGBTOHSV(R, G, B):
    (H,S,V) = rgb_to_hsv(R, G, B)
    return int(H*360), int(S*255), int(V*255)

def RGB_2_HSV(RGB):
    ''' Converts an integer RGB tuple (value range from 0 to 255) to an HSV tuple '''

    # Unpack the tuple for readability
    R, G, B = RGB

    # Compute the H value by finding the maximum of the RGB values
    RGB_Max = max(RGB)
    RGB_Min = min(RGB)

    # Compute the value
    V = RGB_Max;
    if V == 0:
        H = S = 0
        return (H,S,V)


    # Compute the saturation value
    S = 255 * (RGB_Max - RGB_Min) // V

    if S == 0:
        H = 0
        return (H, S, V)

    # Compute the Hue
    if RGB_Max == R:
        H = (360 + 43*(G - B)//(RGB_Max - RGB_Min)) % 360
    elif RGB_Max == G:
        H = 85 + 43*(B - R)//(RGB_Max - RGB_Min)
    else: # RGB_MAX == B
        H = 171 + 43*(R - G)//(RGB_Max - RGB_Min)

    return (H, S, V)

while True:
    R = randint(0, 255)
    G = randint(0, 255)
    B = randint(0, 255)
    print(f"Colours:{R}, {G}, {B}", RGB_2_HSV((R, G, B)), " Actual:", accRGBTOHSV(R/255, G/255, B/255))
    input()