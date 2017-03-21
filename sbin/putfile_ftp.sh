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

logfile=$localPath/ftp_${a}_${b}.log

echo "error" > ${result}

expect <<!

set timeout 60

log_file $logfile

spawn -noecho ftp -i $ftpIp $ftpPort

expect { 
timeout {exit 1}
"onnection refused" {exit 1}
"ervice not available" {exit 1}
"*ame"
}

send "$ftpUser\r"

expect {
timeout  {exit 1}
"*assword:"
}

send "$ftpPwd\r"

expect {
timeout {exit 1}
"*assword" {exit 1}
"ogin failed" {exit 1}
"ftp>"
}

send "binary\r"

#设定lcd,cd
expect "ftp>"

send "lcd $localPath\r"

expect "ftp>"

send "cd $ftpRemotePath\r"

expect {
timeout {exit 1}
"ftp>"
}

send "delete $fileName\r"

expect {
timeout {exit 1}
"ftp>"
}

send "delete ${fileName}.tmp\r"

expect {
timeout {exit 1}
"ftp>"
}
#上传文件
send "put $fileName  ${fileName}.tmp\r"

expect {
timeout {exit 1}
"ailed to open file" {exit 1}
"ftp>"
}

send "rename ${fileName}.tmp $fileName\r"

expect {
timeout {exit 1}
"ailed to open file" {exit 1}
"ftp>"
}

send "close\r"

expect "ftp>"

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
