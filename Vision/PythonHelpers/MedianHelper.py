#matrix =    [[54, 23, 654],
#            [234, 534, 13],
#            [345, 765, 23]]
matrix =    [[4, 6, 4],
            [3, 6, 9],
            [345, 5, 232]]
tempArray = [0]*9
temp = 0
tempArrayIndex = 0
count = 0
#Need to find the 5th largest
for i in range(3):
    for j in range(3):
        #Insert into end of list
        tempArray[8-tempArrayIndex] = matrix[i][j]
        #Bubble sort array
        for k in range(8-tempArrayIndex, 8):
            count += 1
            if tempArray[k] > tempArray[k+1]:
                temp = tempArray[k+1]
                tempArray[k+1] = tempArray[k]
                tempArray[k] = temp
        tempArrayIndex += 1
print(tempArray)
print(count)