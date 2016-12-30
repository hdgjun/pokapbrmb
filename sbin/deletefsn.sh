#!/bin/bash

path=$1
date=$2

echo "path = $path"

for i in `ls ${path} -F|grep "/$"`;
    do echo "dir = $i";
      for h in `ls ${path}/${i} -F|grep "/$"`;
        do echo "h dir = ${h}"
         for k in `ls ${path}/${i}/${h} -F|grep "/$"`;
             do echo "date  = $k";
               if [ ${k:0:8} -lt $date ]
               then  echo "$k < $date";
                   rm -rf ${path}/${i}/${h}/${k};
               else  echo "$k > $date"; 
              fi   
            done;
        done
    done;
