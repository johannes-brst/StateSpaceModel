import matplotlib.pyplot as plt
import matplotlib.cbook as cbook
import shutil
import csv

import numpy as np

YSim = []
realPlant = []
t = []

realPlantFilePath = 'csv/realPlant.csv'
simlationFilePath = 'csv/simulatedOutputSequenceFileOutput.csv'

with open(realPlantFilePath,'r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    row_number = 0
    for row in plots:
        t.append(row_number)
        realPlant.append(float(row[0]))
        row_number+=1
with open(simlationFilePath,'r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    row_number = 0
    for row in plots:
        t.append(row_number)
        YSim.append(float(row[0]))
        row_number+=1

result = 0
for i in range(len(realPlant)):  
    result =+ np.abs((realPlant[i] - YSim[i])/realPlant[i])

result = result / len(realPlant)
result = result * 100
print("MAPE = " + str(result) + "\n")

shutil.copyfile(realPlantFilePath, realPlantFilePath.split(".")[0]+"MAPE="+str(round(result,4))+".csv")
shutil.copyfile(simlationFilePath, simlationFilePath.split(".")[0]+"MAPE="+str(round(result,4))+".csv")

plt.plot(realPlant, label='Ur5e')
plt.plot(YSim, label='Simluation')
plt.xlabel('t')
plt.ylabel('joint[0] angle')
plt.legend()
#plt.show()
plt.savefig("MAPE="+str(round(result,4))+".png")