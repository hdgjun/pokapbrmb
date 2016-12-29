#!/bin/sh

path=/home/poka/pokapbrmb/
shellpath=/home/poka/pokapbrmb/pokapbrmb.sh
procname=pokapbrmb

#stop process
cd ${path}
${shellpath} stop 

#start process
chmod +x *
${shellpath} start
${shellpath} start
echo "Congratulations! ${procname} start success" 

	