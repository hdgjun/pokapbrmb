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
filetype=$8
model=$9

logfile=ftp_${a}_${b}.log

echo "open ${ip} ${port}
passive off
prompt off
user ${user} ${pwd}
binary
cd ${rdir}
ls *${filetype}
close
bye
"|ftp -v -n > ${logfile}


str=`cat ${logfile}|grep Windows_NT`
echo str = $str

if [ -z "$str" ]
then
   for i in `cat ${logfile}|sed -n '/Here comes the directory listing/,/226 Directory send OK/'p|grep ^'[-]'|awk '{print $9}'`;
    do echo $i >>$list;
     done
else
   for i in `cat ${logfile}|sed -n '/Transfer starting/,/Transfer complete/'p|grep -v Transfer|grep -v '<DIR>'|awk '{print $4}'`;
   do echo $i >>$list;
   done
fi

echo "open ${ip} ${port}
passive ${model}
prompt off
user ${user} ${pwd}
binary
cd ${rdir}
lcd ${ldir}
mget *${filetype}
close
bye
"|ftp -v -n > ${logfile}

rm ${logfile}
