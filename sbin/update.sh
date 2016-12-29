#!/bin/sh

path=/home/poka/pokapbrmb/
shellpath=/home/poka/pokapbrmb/pokapbrmb.sh

#compile and start process
chmod +x ${path}/*.sh
cd ${path}
make clean
make
#${shellpath} stop 
${shellpath} start