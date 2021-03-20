import matplotlib.pyplot as plt
import matplotlib.cbook as cbook
import csv

import numpy as np

YSim = []
YY = []
t = []

with open('csv/YY.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    row_number = 0
    for row in plots:
        t.append(row_number)
        YY.append(float(row[0]))
        row_number+=1
with open('csv/simulatedOutputSequenceFileOutput_save.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    row_number = 0
    for row in plots:
        t.append(row_number)
        YSim.append(float(row[0]))
        row_number+=1

result = 0
for i in range(len(YY)):  
    result =+ np.abs((YY[i] - YSim[i])/YY[i])

result = result / len(YY)
result = result * 100
print("MAPE = " + str(result) + "\n")

plt.plot(YY, label='Ur5e')
plt.plot(YSim, label='Simluation')
plt.xlabel('t')
plt.ylabel('joint[0] angle')
plt.legend()
plt.show()