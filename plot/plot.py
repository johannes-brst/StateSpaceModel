import matplotlib.pyplot as plt
import matplotlib.cbook as cbook
import csv

import numpy as np

t = []
YY = []

with open('csv/YY.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    row_number = 0
    for row in plots:
        t.append(row_number)
        YY.append(float(row[0]))
        row_number+=1
        
plt.plot(t,YY, label='Loaded from file!')
plt.xlabel('t')
plt.ylabel('YY')
plt.title('Interesting Graph\nCheck it out')
plt.legend()
plt.show()