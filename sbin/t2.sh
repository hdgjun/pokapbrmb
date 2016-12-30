#!/bin/bash




echo "open 192.168.125.12 
passive off
prompt off
user root 123456
binary
cd /home
close
bye
"|ftp -v -n > unix.log
