/*
 * dbmonboxaddmon.c
 *
 *  Created on: 2017Äê1ÔÂ9ÈÕ
 *      Author: Administrator
 */
#include "db/db.h"
#include "db/dbmonboxaddmon.h"
#include "common.h"
#include "switch.h"
int DbMonboxaddmon(int oprType, MONBOXADDMON *record)
{
	POKA_MYSQL

	char strSql[1024] = {0};

	MONBOXADDMON  tmpData;
	if(record)
	{
		memset(&tmpData, 0x00, sizeof(MONBOXADDMON));
		memcpy(&tmpData, record, sizeof(MONBOXADDMON));
	}

	switch (oprType)
	{
		case DBS_INSERT:
#ifdef BFNEW
		sprintf(strSql,"INSERT INTO MONBOXADDMON(SCANID,FLAG,MONBOXID,REMARK,OPERTIME,OPERATORID,CHECKER,FILENAME,ID) \
				VALUES('%s', '0', '%s', '%s', str_to_date('%s','%s'), '%s','%s','%s','%s')",
				tmpData.scanid,
				tmpData.monboxid,
				"",
				tmpData.opertime,
				"%Y-%m-%d %H:%i:%s",
				tmpData.operatorid,
				tmpData.checker,
				tmpData.filename,
				tmpData.id
				);
#else
		sprintf(strSql,"INSERT INTO MONBOXADDMON(SCANID,FLAG,MONBOXID,REMARK,OPERTIME,OPERATORID,CHECKER,FILENAME) \
						VALUES('%s', '0', '%s', '%s', str_to_date('%s','%s'), '%s','%s','%s')",
						tmpData.scanid,
						tmpData.monboxid,
						"",
						tmpData.opertime,
						"%Y-%m-%d %H:%i:%s",
						tmpData.operatorid,
						tmpData.checker,
						tmpData.filename
						);
#endif
		mysql_query(pcon, strSql);
		return JudgeSqlExecResultLocal(0,"DBS_INSERT ",pcon);
	}
	return SUCESS;
}

