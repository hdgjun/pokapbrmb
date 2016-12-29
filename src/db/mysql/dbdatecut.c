/*
 * dbdatecut.c
 *
 *  Created on: 2016Äê12ÔÂ26ÈÕ
 *      Author: Administrator
 */

#include "db/db.h"
#include "db/dbdatecut.h"
#include "common.h"
int DbDateCut(int oprType, DATECUT *re)
{
	POKA_MYSQL
	DATECUT  tmpData;
	char strSql[300] = {0};

	memset(&tmpData, 0x00, sizeof(DATECUT));
	memcpy(&tmpData, re, sizeof(DATECUT));
	MYSQL_ROW row;
	int iRet;
	unsigned long ulrow = 0;
	switch (oprType)
	{
		case DBS_SELECT:
			mysql_query(pcon,"SELECT ID,LASTDATE,CURDATE,STATUS,CUtTIME,ENDTIME FROM DATECUT");
			iRet = JudgeSqlExecResultLocal(0,"DBS_SELECT  DATECUT",pcon);
			if(iRet != SUCESS)return iRet;

			ctx->result =  mysql_store_result(pcon);
			ulrow = (unsigned long)mysql_num_rows(ctx->result);
			if(ctx->result&&ulrow>0)
			{
				row = mysql_fetch_row(ctx->result);
				if(row[0]!=NULL)tmpData.id = atoi(row[0]);
				if(row[1]!=NULL)tmpData.lastdate = atoi(row[1]);
				if(row[2]!=NULL)tmpData.curdate = atoi(row[2]);
				if(row[3]!=NULL)tmpData.status = atoi(row[3]);
				if(row[4]!=NULL)tmpData.cuttime = atoi(row[4]);
				if(row[5]!=NULL)tmpData.endtime = atoi(row[5]);
				mysql_free_result(ctx->result);
				memcpy(re,&tmpData,sizeof(DATECUT));
				return SUCESS;
			}else{
				return ERROR;
			}
		case DBS_INSERT:
			sprintf(strSql,"INSERT INTO  DATECUT(LASTDATE,CURDATE,STATUS,CUTTIME,ENDTIME) \
							VALUES('%d','%d','%d','%d','%d') ",tmpData.lastdate,
							tmpData.curdate,tmpData.status,tmpData.cuttime,tmpData.endtime);
			mysql_query(pcon,strSql);
			return JudgeSqlExecResultLocal(0,"DBS_INSERT  DATECUT",pcon);

		case DBS_UPDATE:
			sprintf(strSql,"UPDATE  DATECUT SET LASTDATE='%d',CURDATE='%d',STATUS='%d',CUTTIME='%d',ENDTIME='%d' "
			,tmpData.lastdate,tmpData.curdate,tmpData.status,tmpData.cuttime,tmpData.endtime);
			mysql_query(pcon,strSql);
			return JudgeSqlExecResultLocal(0,"DBS_UPDATE  DATECUT ",pcon);
	}
	return SUCESS;
}
