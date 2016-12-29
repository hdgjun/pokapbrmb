#!/bin/bash

paramnum=3

echo "Shell script path 		= $0"
echo "There are $# param(s):"
echo "Lower bank IP 			= $1"
echo "Lower bank old fsn path 	= $2"
echo "Lower bank new fsn path 	= $3"

if [ ${paramnum} != $# ]
then
echo "Param num error,right num:${paramnum} actual num:$#"
fi

username=root
userpasswd=123456

echo "
open $1   
passive off
prompt off
user ${username} ${userpasswd}
binary
pwd
rename $2 $3 
close
bye
" |ftp -v -n |tee autoftp.log 
SUCC="226 File send OK."||"226 Transfer complete."
LOCAL="local:"
RESULT=`grep -I "${SUCC}" autoftp.log` 

#cat autoftp.log |grep "Opening" |awk '{print $8}'
#cat autoftp.log | awk 'NF==10{print $8,$9,$10}'|awk '{print $1,$2}'|sed 's/(//' >fileinfo.txt
#cat autoftp.log |awk 'NF==3{print $1,$2,$3}'>>fileinfo.txt
#rm -rf autoftp.log

if [ "${RESULT}" = "${SUCC}" ]
then
exit 0
else
exit 1
fi
