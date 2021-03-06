#!/bin/bash

ftpIp=$1
ftpPort=$2
localPath=$3
ftpRemotePath=$4
ftpUser=$5
ftpPwd=$6
fileName=$7
result=$8
model=$9


a=`date +%s`

b=`date +%s`

logfile=${localPath}/ftp_${a}_${b}.log

echo "error" > ${result}

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
#上传文件
send "put $fileName  ${fileName}.tmp\n"
expect "sftp>"
send "rename ${fileName}.tmp $fileName\n"

expect "sftp>"

send "bye\r"
interact
expect eof
!

str=`cat ${logfile}|grep 100%`
if [ -z "$str" ]
then
       echo "error" > ${result}
else
      echo "sucess" > ${result};
fi

rm $logfile
