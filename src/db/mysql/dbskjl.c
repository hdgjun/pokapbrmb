/*
 * dbskjl.c
 *
 *  Created on: 2017年1月13日
 *      Author: Administrator
 */
#include "db/db.h"
#include "db/dbskjl.h"
#include "common.h"
#include "pokalog.h"
#include "filepath.h"

static const char insertsql[] = "INSERT DELAYED INTO SKJL(\
		BANKNO,NETNO,USERID,PERCODE,BUSINESSDATE,ACCOUNTNO,INSERTDATE) VALUES(";

int DbSKJL(int oprType, DBSKJL *record)
{
	POKA_MYSQL

	char strSql[300+FILE_PATH_CHARNUM*21] = {0};

	DBSKJL  tmpData;

	memset(&tmpData, 0x00, sizeof(DBSKJL));
	memcpy(&tmpData, record, sizeof(DBSKJL));

	switch (oprType)
	{
		case DBS_INSERT:
			sprintf(strSql, "%s '%s','%s','%s','%s',str_to_date('%s','%s'),'%s','NOW()')", insertsql,
				tmpData.bankno,
				tmpData.notno,
				tmpData.userid,
				tmpData.percode,
				tmpData.businessdate,//SYSDATE,	//记录时间
				"%Y-%m-%d %H:%i:%s",
				tmpData.accountno);
			 mysql_query(pcon, strSql);
			return JudgeSqlExecResultLocal(0,"DBS_INSERT ",pcon);
	}
   return SUCESS;
}
