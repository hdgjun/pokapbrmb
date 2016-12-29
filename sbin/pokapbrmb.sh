#!/bin/sh

daemonname=pokadaemon
procname=pokapbrmb

basepath=/home/poka/xiang/pokapbrmb/
daemonpath=/home/poka/xiang/pokapbrmb/pokadaemon
procpath=/home/poka/xiang/pokapbrmb/pokapbrmb

#oraclehomepath=/opt/oracle/product/11gR2/db
#oracletnsadmin=/opt/oracle/product/11gR2/db/network/admin
oraclehomepath=/opt/oracle/product/10gR1/db
oracletnsadmin=/opt/oracle/product/10gR1/db/network/admin
oraclesid=orcl

case "$1" in
	start)
		if test $( pgrep -f ${procname} | wc -l ) -gt 2 
		then 
		echo "${procname} is runing!" 
		else
		export ORACLE_HOME=${oraclehomepath}
		export ORACLE_SID=${oraclesid}
		export TNS_ADMIN=${oracletnsadmin}
		cd ${basepath}
		nohup ${procpath} >/dev/null &
		nohup ${daemonpath} >/dev/null &
		fi
		;;
	stop)
		ps -ef | grep -E ${daemonname} | grep -v grep | awk '{print $2}' | xargs kill -9
		ps -ef | grep -E ${procname} | grep -v grep | awk '{print $2}' | xargs kill -9
		;;
esac

exit



