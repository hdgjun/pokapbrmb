#!/bin/sh

path=/home/poka/pokapbrmb/
shellpath=/home/poka/pokapbrmb/pokapbrmb.sh

autoshellname=poka_sh
atuopath=/etc/init.d/


#add power on
cp ${path}/${autoshellname}  ${atuopath}
chmod +x ${atuopath}/${autoshellname}
chkconfig -add ${autoshellname}

#start process
chmod +x ${path}/*.sh
cd ${path}
make clean
make
#${shellpath} stop 
${shellpath} start
	