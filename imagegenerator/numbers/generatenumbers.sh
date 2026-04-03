#!/bin/bash
#put this file in folder with number png files and run

for i in elevatorInd_5x8/*.png
do
        
        echo "$i"
		filenamewopath=$(basename -- "$i")
		filename="${filenamewopath%.*}"
		echo "$filename"
		python ../generate.py $i -f nodered -o $filename.txt
done