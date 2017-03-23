#!/bin/sh

daemonname=pokadaemon
procname=pokapbrmb


basepath=/home/poka/pokapbrmb/


case "$1" in
	start)
                if [ -z "${POKA_HOME}" ];
                then
                   echo "export poka_home" 
                   export POKA_HOME=$basepath;
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



