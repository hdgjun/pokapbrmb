#!/bin/sh

paramnum=4

echo "Shell script path 			= $0"
echo "There are $# param(s):"
echo "Upper bank IP 				= $1"
echo "Upper bank update file path 	= $2"
echo "Local save file path 			= $3"
echo "Get file name 				= $4"

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
-mget $4 .
quit
EOF
