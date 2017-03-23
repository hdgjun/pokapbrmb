#!/bin/bash
POKA_HOME=/home/poka/pokapbrmb/
DSConfigFile=$POKA_HOME/cfg/pkrmb.ini

DBNAME=`awk 'FS="=" {if ($0~/^dbservicename/) print $2}' $DSConfigFile`
DBUSR=`awk 'FS="=" {if ($0~/^dbuser/) print $2}' $DSConfigFile`
DBPWD=`awk 'FS="=" {if ($0~/^dbpassword/) print $2}' $DSConfigFile`
dbname=$DBNAME
user=$DBUSR
passwd=$DBPWD

echo "dbname $dbname"
echo "user $user"
echo "passwd $passwd"

TAB_NAME=$1

SCH_NAME=$user

db2 connect to $DBNAME user $DBUSR using $DBPWD >/dev/null

for index in {0..2}

do

nextdate=`db2 -tx "select to_char(current date+"${index}" day,'yyyymmdd')  from sysibm.sysdummy1"`
nextdate=`echo ${nextdate}|sed s/[[:space:]]//g`

echo "nextdate $nextdate"

partition_name='PART_'${nextdate}

echo "partition_name=$partition_name"

vi_result=`db2 -tx "select count(*) from syscat.datapartitions t where tabschema = upper('"$SCH_NAME"') 
                                                      and tabname = upper('"$TAB_NAME"')
                                                      and datapartitionname=upper('"$partition_name"') "`

vi_result=`echo $vi_result|sed s/[[:space:]]//g`
echo "vi_result=$vi_result"

if [ $vi_result = 0 ]
then
   ndate=`db2 -tx "select to_char(current date+"${index}" day,'yyyy-mm-dd')  from sysibm.sysdummy1"`
   ndate=`echo ${ndate}|sed s/[[:space:]]//g`
   start_time=${ndate}'-00.00.00.000000'
   echo "start_time=$start_time"
   end_time=${ndate}'-23.59.59.999999'
   echo "end_time=${end_time}"

   #add_partition="alter table $SCH_NAME.$TAB_NAME add partition $partition_name  STARTING '"$start_time"' INCLUSIVE ENDING  '"$end_time"' exclusive "
   add_partition=`db2 -tx "alter table $SCH_NAME.$TAB_NAME add partition $partition_name  STARTING '"$start_time"' INCLUSIVE ENDING  '"$end_time"' exclusive "`

  echo "add_partition $add_partition"
else
  echo "partition $partition_name exist"   
fi

done
