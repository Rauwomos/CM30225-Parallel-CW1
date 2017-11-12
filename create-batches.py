from string import Template

# Templates
submitTemplate = Template('#!/bin/sh\n#SBATCH --account=cm30225\n#SBATCH --partition=teaching\n#SBATCH --job-name=${job_name}\n#SBATCH --nodes=1\n#SBATCH --time=00:15:00\n#SBATCH --output=${job_name}.out\n${run}')
runSingleTemplate = Template('./single-checkerboard -s $size -u 1 -d 3 -l 4 -r 2 -p 0.00001')
runMultiTemplate = Template('./multi-checkerboard -s $size -u 1 -d 3 -l 4 -r 2 -p 0.00001 -t $thread_count')
jobNameTemplate = Template('rw-${thread_count}-${size}')
fileNameTemplate = Template('submit-${thread_count}-${size}.slm')

def createSubmitFile(threadCount, problemSize):
    if threadCount == 1:
        run = runSingleTemplate.substitute(size=size)
    else:
        run = runMultiTemplate.substitute(size=size, thread_count=threadCount)
    jobName = jobNameTemplate.substitute(thread_count=threadCount, size=size)
    fileContents = submitTemplate.substitute(job_name=jobName, run=run)
    fileName = fileNameTemplate.substitute(thread_count=threadCount, size=size)
    with open(fileName, 'w') as f:
        f.write(fileContents)


# problemSizes = [10,50,100,150,200,250,300,350,400,450,500,600,700,800,900,1000]
# threadCounts = [1,2,4,6,8,10,12,14,16]
problemSizes = [10,50]
threadCounts = [1,2]

for threadCount in threadCounts:
    for size in problemSizes:
        createSubmitFile(threadCount, size)
