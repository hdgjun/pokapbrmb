#!/bin/bash

#oracle连接的数据库名称
OracleUser=system
OraclePassword=123456
OracleServiceName=orcl
delprocname=system.deletehisdata
DelMonsSpace=6

#oraclehomepath=/opt/oracle/product/11gR2/db
#oracletnsadmin=/opt/oracle/product/11gR2/db/network/admin
#oraclehomepath=/opt/oracle/product/10gR1/db
#oracletnsadmin=/opt/oracle/product/10gR1/db/network/admin
#oraclesid=orcl
source /etc/profile
#source /home/oracle/.profile

echo "connect oracle start"

#export ORACLE_HOME=${oraclehomepath}
#export ORACLE_SID=${oraclesid}
#export TNS_ADMIN=${oracletnsadmin}
sqlplus ${OracleUser}/${OraclePassword}@${OracleServiceName} << EOF 

call ${delprocname}(${DelMonsSpace});

quit;

echo "disconnect oracle end"

exit;

EOF

