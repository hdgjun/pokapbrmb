/*
 * dbmonrule.c
 *
 *  Created on: 2017Äê1ÔÂ12ÈÕ
 *      Author: Administrator
 */
#include "common.h"
#include "db/db.h"
#include "db/dbmonrule.h"

int DbMonRule(int oprType, MONRULEARRY *re)
{
	POKA_MYSQL

	char strSql[300] = {0};

	MONRULEARRY  tmpData;

	if(re)
	{
		memset(&tmpData, 0x00, sizeof(MONRULEARRY));
		memcpy(&tmpData, re, sizeof(MONRULEARRY));
	}
	unsigned long ulrow = 0;
	MYSQL_ROW row;
	switch (oprType)
	{
		case DBS_SELECT:
			mysql_query(pcon,"SELECT MONHEAD,IFNULL(MONTYPE,'0'),MONVAL FROM MONRULE WHERE MONSTATUS='0'");
			ctx->result =  mysql_store_result(pcon);
			ulrow = (unsigned long)mysql_num_rows(ctx->result);
			if(ulrow <=0){
				tmpData.size = 0;
				if(ctx->result)
				mysql_free_result(ctx->result);
				memcpy(re,&tmpData,sizeof(MONRULEARRY));
				return SUCESS;
			}else{

				int i = 0;
				while((row = mysql_fetch_row(ctx->result)))
				{
					if(row[0]!=NULL)memcpy(&tmpData.data[i].MonHead,row[0],strlen(row[0]));
					if(row[1]!=NULL)memcpy(&tmpData.data[i].MonVer,row[1],strlen(row[1]));
					if(row[2]!=NULL)memcpy(&tmpData.data[i].MonVal,row[2],strlen(row[2]));

					i++;
				}
				tmpData.size = i;
				memcpy(re,&tmpData,sizeof(MONRULEARRY));
				if(ctx->result)
					mysql_free_result(ctx->result);
			}
		return JudgeSqlExecResultLocal(0,"DBS_SELECT  MONRULE",pcon);
	}
	return SUCESS;
}
