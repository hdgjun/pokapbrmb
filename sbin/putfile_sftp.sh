#!/bin/sh

paramnum=6

echo "Shell script path 		= $0"
echo "There are $# param(s):"
echo "Upper bank IP 			= $1"
echo "Upper bank fsn file path 	= $2"
echo "Local fsn file path 		= $3"
echo "Send fsn name 			= $4"
echo "Upper bank old fsn path 	= $5"
echo "Upper bank new fsn path 	= $6"

if [ ${paramnum} != $# ]
then
echo "Param num error,right num:${paramnum} actual num:$#"
fi

echo $1
sftp root@$1 << EOF
pwd
cd $2
pwd
lcd $3
-mput $4 .
rename $5 $6
quit
EOF
