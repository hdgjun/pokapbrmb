#!/bin/sh

daemonname=pokadaemon
procname=pokapbrmb


basepath=/home/poka/pokapbrmb/

#oraclehomepath=/opt/oracle/product/11gR2/db
#oracletnsadmin=/opt/oracle/product/11gR2/db/network/admin
oraclehomepath=/opt/oracle/product/10gR1/db
oracletnsadmin=/opt/oracle/product/10gR1/db/network/admin
oraclesid=orcl

case "$1" in
	start)
                if [ -z "${POKA_HOME}" ];
                then
                   echo "export poka_home" 
                   export POKA_HOME=$basepath;
                fi
                
                if [ -z "${ORACLE_HOME}" ];
                then
                   export ORACLE_HOME=${oraclehomepath}
                fi
  
                if [ -z "${ORACLE_SID}" ];
                then 
                   export ORACL_SID=${oraclesid} 
                fi

                if [ -z "${TNS_ADMIN}" ]
                then 
                   export TNS_ADMIN=${oracletnsadmin}
                fi
                
                if test $( pgrep -f ${daemonname} |wc -l ) -eq 1
                then "{daemonname} is running!"
                else
                     cd ${POKA_HOME}/bin/
                     nohup ${POKA_HOME}/bin/${daemonname} > /dev/null &  
                fi
               
		;;
	stop)
		pgrep -f ${daemonname} | xargs kill -9
		pgrep -f ${procname}   | xargs kill -9
		;;
esac

exit



