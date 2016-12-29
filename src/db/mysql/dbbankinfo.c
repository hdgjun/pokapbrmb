/*
 * dbbankinfo.c
 *
 *  Created on: 2016Äê12ÔÂ27ÈÕ
 *      Author: Administrator
 */
#include "db/db.h"
#include "db/dbbankinfo.h"
#include "common.h"

int DbBankinfo(int oprType, BANKINFO *record)
{
	POKA_MYSQL

	MYSQL_ROW row;
	char strSql[300+FILE_PATH_CHARNUM*21] = {0};
	BANKINFO temData;
	if(record)
	{
		memset(&temData,0x00,sizeof(BANKINFO));
		memcpy(&temData,record,sizeof(BANKINFO));
	}
	int iRet;
	unsigned long ulrow = 0;
	switch (oprType)
	{
		case DBS_SEL_ONE_ROW:
			sprintf(strSql,"SELECT IPADDR,IPPORT FROM BANKINFO WHERE BANKNO='%s' ",temData.bankno);
			mysql_query(pcon,strSql);
			ctx->result =  mysql_store_result(pcon);
			row = mysql_fetch_row(ctx->result);
			if(row){
				if(row[0]!=NULL)memcpy(temData.ipaddr,row[0],strlen(row[0]));
				if(row[1]!=NULL)memcpy(temData.ipport,row[1],strlen(row[1]));
				memcpy(record,&temData,sizeof(BANKINFO));
				if(ctx->result)mysql_free_result(ctx->result);
				return SUCESS;
			}else{
				if(ctx->result)mysql_free_result(ctx->result);
				return ERROR;
			}
	}

	return SUCESS;
}

