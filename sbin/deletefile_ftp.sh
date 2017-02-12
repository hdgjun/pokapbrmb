#!/bin/bash

a=`date +%s`

b=`date +%s`

ip=$1
port=$2
rdir=$3
user=$4
pwd=$5
list=$6
filename=$7
model=$8

logfile=ftp_${a}_${b}.log


echo "open ${ip} ${port}
passive ${model}
prompt off
user ${user} ${pwd}
binary
cd ${rdir}
delete ${filename}
close
bye
"|ftp -v -n > ${logfile}

rm ${logfile}
