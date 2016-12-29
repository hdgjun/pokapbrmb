#!/bin/bash

paramnum=8

t={date +%s}
echo "t  = $t"

echo "Shell script path 		= $0"
echo "There are $# param(s):"
echo "bank IP 			        = $1"
echo "ftp port          	    = $2"
echo "ftp user          		= $3"
echo "ftp pwd       			= **"
echo "remote dir            	= $5"
echo "local dir             	= $6"
echo "logname    	            = $7"
echo "filename                  = $8"

if [ ${paramnum} != $# ]
then
echo "Param num error,right num:${paramnum} actual num:$#"
fi
time={date+%s}

echo "
open $1  $2 
passive off
prompt off
user $3 $4
binary
pwd
cd $5
pwd
lcd $6
put $8 ${8}.${time} 
rename ${8}.${time} $8
close
bye
" |ftp -v -n |tee ${7} 

exit 0
