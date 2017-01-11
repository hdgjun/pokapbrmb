#!/bin/bash

ftpIp=$1
ftpPort=$2
localPath=$3
ftpRemotePath=$4
ftpUser=$5
ftpPwd=$6
list=$7
fileName=$8
model=$9


if [ $fileName -eq 1 ]
then 
     fileName="\*"
else
     fileName="\*"$8
fi

echo fileName $fileName


a=`date +%s`

b=`date +%s`

logfile=${localPath}/ftp_${a}_${b}.log


expect <<!

set timeout 60

log_file $logfile

spawn sftp -oport=$ftpPort $ftpUser@$ftpIp


expect {  
  "(yes/no)?" {send "yes\n"
               expect "*assword:" {send "$ftpPwd\n"}
              }
  "*assword:" {send "$ftpPwd\n"}
}


expect "sftp>"

#设定lcd,cd
send "lcd $localPath\n"
send "cd $ftpRemotePath\n"

expect "sftp>"
send "mget ${fileName} \n"

expect "sftp>"
send "bye\n"

interact
expect eof
!
echo list $list
echo logfile $logfile
str=`cat ${logfile}|grep 100%|awk '{print $1}'`
echo "$str" >${list}

rm $logfile

