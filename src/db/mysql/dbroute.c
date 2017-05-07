/*
 * dbroute.c
 *
 *  Created on: 2016Äê12ÔÂ27ÈÕ
 *      Author: Administrator
 */
#include "db/db.h"
#include "db/dbroute.h"
#include "common.h"
#include "pokalog.h"
#include "filepath.h"
int DbRoute(int oprType, ROUTE *record)
{
	POKA_MYSQL

	MYSQL_ROW row;
	char strSql[300+FILE_PATH_CHARNUM*21] = {0};
	ROUTE temData;
	if(record!=NULL){
		memset(&temData,0x00,sizeof(ROUTE));
		memcpy(&temData,record,sizeof(ROUTE));
	}
	int iRet;
	unsigned long ulrow = 0;
	switch (oprType)
	{
		case DBS_CURSOR_OPEN:
		sprintf(strSql,"SELECT ID,TARGETBANK,FORWARDBANK,USER_,PASSWORD_,\
				LOCALDIR,REMOTEDIR,TYPE_,SERVICECODE,STARTTIME,INTER_VAL ,\
				STATUS_,LASTDATE,IPADDR,PORT_,MODEL_ FROM ROUTE WHERE ((STATUS_ = 1 OR STATUS_ IS NULL) \
				AND INTER_VAL = 0 AND STARTTIME < %d  AND (LASTDATE <> '%d' OR LASTDATE IS NULL)) \
				OR ((STATUS_ = 1 OR STATUS_ IS NULL) AND (INTER_VAL <> 0))",temData.starttime,temData.lastdate);
		mysql_query(pcon,strSql);
		printf("%s",strSql);
		ctx->result =  mysql_store_result(pcon);
		pthread_setspecific(p_Thread_key,(void *)(ctx));
		return JudgeSqlExecResultLocal(0,"DBS_CURSOR_OPEN ",pcon);
		case DBS_FETCH:
			if(ctx->result){
				row = mysql_fetch_row(ctx->result);
				if(row){
					if(row[0]!=NULL)temData.id = atoi(row[0]);
					if(row[1]!=NULL)memcpy(temData.targetbank,row[1],strlen(row[1]));
					if(row[2]!=NULL)memcpy(temData.forwardbank,row[2],strlen(row[2]));
					if(row[3]!=NULL)memcpy(temData.user,row[3],strlen(row[3]));
					if(row[4]!=NULL)memcpy(temData.password,row[4],strlen(row[4]));
					if(row[5]!=NULL)memcpy(temData.localdir,row[5],strlen(row[5]));
					if(row[6]!=NULL)memcpy(temData.remotedir,row[6],strlen(row[6]));
					if(row[7]!=NULL)temData.type= atoi(row[7]);
					if(row[8]!=NULL)temData.servicecode = atoi(row[8]);
					if(row[9]!=NULL)temData.starttime= atoi(row[9]);
					if(row[10]!=NULL)temData.interval= atoi(row[10]);
					if(row[11]!=NULL)temData.status= atoi(row[11]);
					if(row[12]!=NULL)temData.lastdate= atoi(row[12]);
					if(row[13]!=NULL)memcpy(temData.ipaddr,row[13],strlen(row[13]));
					if(row[14]!=NULL)memcpy(temData.port,row[14],strlen(row[14]));
					if(row[15]!=NULL)temData.model= atoi(row[15]);
					strtrim(temData.targetbank);
					strtrim(temData.forwardbank);
					strtrim(temData.user);
					strtrim(temData.password);
					strtrim(temData.localdir);
					strtrim(temData.remotedir);
					strtrim(temData.ipaddr);
					strtrim(temData.port);
					memcpy(record,&temData,sizeof(ROUTE));
					pthread_setspecific(p_Thread_key,(void *)(ctx));
					return SUCESS;
				}else{
					return NODATA;
				}
			}return ERROR;
		case DBS_CLOSE:
			if(ctx->result)
			mysql_free_result(ctx->result);

			pthread_setspecific(p_Thread_key,(void *)(ctx));
			return SUCESS;
		case DBS_UPDATE:
			sprintf(strSql,"UPDATE ROUTE SET STATUS_ = '%c' ,STARTTIME = %d,LASTDATE = '%d' where ID='%d'",
					temData.status,temData.starttime,temData.lastdate,temData.id);
			mysql_query(pcon,strSql);
			return JudgeSqlExecResultLocal(0,"DBS_UPDATE ",pcon);
		case DBS_UPDATE1:
			mysql_query(pcon,"UPDATE ROUTE SET STATUS_ = '1'  where STATUS_='0'");
		return JudgeSqlExecResultLocal(0,"DBS_UPDATE1 ",pcon);
		case DBS_UPDATE1:
			mysql_query(pcon,"UPDATE ROUTE SET STARTTIME = 1  where STARTTIME > 230000 and (INTER_VAL is null or INTER_VAL = 0) ");
		return JudgeSqlExecResultLocal(0,"DBS_UPDATE2 ",pcon);
	}
	return SUCESS;
}
