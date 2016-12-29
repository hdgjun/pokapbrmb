#!/bin/sh

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

echo $1
sftp root@$1 << EOF
pwd
-rename $2 $3 .
quit
EOF
