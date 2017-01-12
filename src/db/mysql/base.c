/*
 * base.pc
 *
 *  Created on: 2016年12月22日
 *      Author: Administrator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include "db/db.h"
#include "allcommon.h"
#include "pokalog.h"

extern Param g_param;
#define INIT_SCR   "initmysql.sql"
int InitDb(char *path)
{
	char cmd[500] = {0};
	pthread_key_create(&p_Thread_key,NULL);
	mysql_library_init(0, NULL, NULL);
	sprintf(cmd,"mysql -h%s -P%s -u%s -p%s %s < %s/%s",
			g_param.DBIP,g_param.DBPort,g_param.DBUser,g_param.DBPassword,
			g_param.DBServiceName,path,INIT_SCR);
	system(cmd);
	return SUCESS;
}
int DestroyDb(){
	mysql_library_end();
	return SUCESS;
}
int ThreadConnectDB()
{
	MYSQL *pcon;
	MYSQLCTX *ctx = malloc(sizeof(MYSQLCTX));

	my_init();

	pcon = malloc(sizeof(MYSQL));
	memset(pcon, 0, sizeof(MYSQL));
    ctx->pcon = pcon;

	char value = 1;
	mysql_options(pcon, MYSQL_OPT_RECONNECT, (char *)&value);

	if (!mysql_real_connect(pcon, g_param.DBIP,
			g_param.DBUser, g_param.DBPassword, g_param.DBServiceName, 0, NULL, CLIENT_MULTI_STATEMENTS|CLIENT_MULTI_RESULTS))
	{
	   printf("Connect sql fail:%s  [%s]\n",mysql_error(pcon),g_param.DBServiceName);
	   return ERROR;
	}
	pthread_setspecific(p_Thread_key,(void *)(ctx));
	return SUCESS;
}
int DisconnectDB(){
	POKA_MYSQL
	mysql_close(pcon);
	if(pcon!=NULL)
		free(pcon);
	free(ctx);
	return SUCESS;
}
int TestConnectionDB()
{
	POKA_MYSQL
	if(pcon == NULL)
	   	return ERROR;
	return mysql_ping(pcon);
}

int DbsCommit()
{
	return SUCESS;
}

int DbsRollback()
{
	return SUCESS;
}

int	JudgeSqlExecResultLocal(int iFlag,char *szExecText,MYSQL *pcon)
{
	int iRet;
	if(pcon){
		iRet = mysql_errno(pcon);
		if(iRet == 2002)
		{/*未连接数据库*/
			vLog("szExecText [%s]  errno:[%d]  err:[%s] \n",szExecText,mysql_errno(pcon),mysql_error(pcon));
			return WARING;
		}else if(iRet != 0)
		{
			vLog("szExecText [%s]  errno:[%d]  err:[%s] \n",szExecText,mysql_errno(pcon),mysql_error(pcon));
			return ERROR;
		}
		printf("szExecText [%s]  errno:[%d]  err:[%s] \n",szExecText,mysql_errno(pcon),mysql_error(pcon));
	}else
	{
		vLog("not connected to db\n");
		return WARING;
	}

	return SUCESS;
}

