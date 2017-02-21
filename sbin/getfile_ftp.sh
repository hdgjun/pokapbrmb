#!/bin/bash

a=`date +%s`

b=`date +%s`

ip=$1
port=$2
ldir=$3
rdir=$4
user=$5
pwd=$6
list=$7
filename=$8
model=$9

logfile=ftp_${a}_${b}.log


echo "open ${ip} ${port}

passive ${model}

prompt off

user ${user} ${pwd}

binary

cd ${rdir}

lcd ${ldir}

get ${filename}

close

bye
"|ftp -v -n > ${logfile}

rm ${logfile}
