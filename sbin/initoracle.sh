#!/bin/bash

user=$1
passwd=$2
service=$3
script=$4

sqlplus ${user}/${passwd}@${service} << EOF
@${script}
exit;
EOF

