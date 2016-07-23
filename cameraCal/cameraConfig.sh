#!/bin/bash
#program will be used to query the number of cameras, and determine which
#camera is which based on the port its plugged into
rm cal.txt
for camera in $( ls /dev/ | grep video); do
  echo item: $camera
  path=$(udevadm info --name=$camera | grep ID_FOR_SEAT)
  echo $path
  substring=$(echo $path | cut -d'b' -f 2)
  echo $substring
  echo $camera >> cal.txt
  echo $substring >> cal.txt
done
