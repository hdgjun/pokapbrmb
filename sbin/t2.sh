#!/bin/bash




echo "open localhost 
passive off
prompt off
user root 123456
binary
ls
close
bye
"|ftp -v -n > 2.log
