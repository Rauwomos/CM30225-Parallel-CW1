import csv
import os
from glob import glob

def print2DArray(array):
    for i in range(len(array)):
        for j in range(len(array[0])):
            print("{}, ".format(array[i][j]),end='')
        print()
    return

dirPath = os.path.dirname(os.path.realpath(__file__)) + '/V4-AVX/'
globPattern = dirPath + "*.out"

problemSizes = [0,10,50,100,150,200,250,300,350,400,450,500,600,700,800,900,1000,1100,1200,1300,1400,1500,1600,2000,2500,3000,3500,4000,4500]
threadCounts = [0,1,2,4,6,8,10,12,14,16]
filePaths = glob(globPattern)

resultsTimings = [['0' for x in range(len(problemSizes))] for y in range(len(threadCounts))] 
resultsIterations = [['0' for x in range(len(problemSizes))] for y in range(len(threadCounts))] 

for i in range(len(threadCounts)):
    resultsTimings[i][0] = threadCounts[i]
    resultsIterations[i][0] = threadCounts[i]

for j in range(len(problemSizes)):
    resultsTimings[0][j] = problemSizes[j]
    resultsIterations[0][j] = problemSizes[j]

for path in filePaths:
    with open(path,'r') as f:
        data = f.read().split('\n')
        if not data[0].split()[0] == "slurmstepd:":
            threadCount = data[0].split()[-1]
            sizeOfPlane = data[1].split()[-1]
            iterations = data[2].split()[-1]
            time = data[3].split()[-1][:-1]

            i = threadCounts.index(int(threadCount))
            j = problemSizes.index(int(sizeOfPlane))
            resultsTimings[i][j] = time
            resultsIterations[i][j] = iterations

with open('timings.csv','w') as f:
    w = csv.writer(f,delimiter=',')
    w.writerows(resultsTimings)

with open('iterations.csv','w') as f:
    w = csv.writer(f,delimiter=',')
    w.writerows(resultsIterations)