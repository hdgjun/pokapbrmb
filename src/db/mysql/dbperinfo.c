/*
 * dbperinfo.c
 *
 *  Created on: 2016Äê12ÔÂ26ÈÕ
 *      Author: Administrator
 */

#include "db/db.h"
#include "db/dbperinfo.h"
#include "common.h"

int DbPerinfo(int oprType, PERINFO *record)
{
	POKA_MYSQL

	MYSQL_ROW row;
	char strSql[300+FILE_PATH_CHARNUM*21] = {0};

	PERINFO tem;
	memset(&tem,0x00,sizeof(PERINFO));
	memcpy(&tem,record,sizeof(PERINFO));

	int iRet;
	unsigned long ulrow = 0;
	switch (oprType)
	{
		case DBS_SELECT:
			sprintf(strSql,"SELECT BANKNO,AGENCYNO,PERCODE FROM PERINFO \
					WHERE TERMINALNO='%s' ",tem.terminalno);
			mysql_query(pcon,strSql);
			iRet = JudgeSqlExecResultLocal(0,"DBS_SELECT ",pcon);
			if(iRet != SUCESS)return iRet;

			ctx->result =  mysql_store_result(pcon);
			ulrow = (unsigned long)mysql_num_rows(ctx->result);
			if(ctx->result&&ulrow>0)
			{
				row = mysql_fetch_row(ctx->result);
				if(row[0]!=NULL)memcpy(tem.bankno,row[0],strlen(row[0]));
				if(row[1]!=NULL)memcpy(tem.agencyno,row[1],strlen(row[1]));
				if(row[2]!=NULL)memcpy(tem.percode,row[2],strlen(row[2]));
				mysql_free_result(ctx->result);
				memcpy(record,&tem,sizeof(PERINFO));
				return SUCESS;
			}
			if(ctx->result)
				mysql_free_result(ctx->result);
			return ERROR;

	}
	return SUCESS;
}
