/*
 * dbbaginfo.c
 *
 *  Created on: 2016Äê12ÔÂ25ÈÕ
 *      Author: Administrator
 */

#include "db/db.h"
#include "db/dbbaginfo.h"
#include "common.h"
int DbBaginfo(int oprType, BAGINFO *fileRecord)
{
	POKA_MYSQL


	BAGINFO  tmpData;
	char strSql[300] = {0};

	memset(&tmpData, 0x00, sizeof(BAGINFO));
	memcpy(&tmpData, fileRecord, sizeof(BAGINFO));

	switch (oprType)
	{
		case DBS_INSERT:
			sprintf(strSql,"INSERT INTO BAGINFO(\
				BAGCODE        ,\
				BANKID         ,\
				BAGDATE        ,\
				BAGBAGID       ,\
				BAGCHECKID     ,\
				BUNDLECODE     ,\
				BUNDLEDATE     ,\
				WRITESTATUS    ,\
				PUTSTATUS	   ,\
				FILENAME )   \
			VALUES('%s','%s',str_to_date('%s','%s'),'%s','%s','%s',str_to_date('%s','%s'),'%c','%c','%s')",
			tmpData.BagCode,
			tmpData.BankId,
			tmpData.BagDate,
			"%Y-%m-%d %H:%i:%s",
			tmpData.BagBagId,
			tmpData.BagCheckId,
			tmpData.BundleCode,
			tmpData.BundleDate,
			"%Y-%m-%d %H:%i:%s",
			'0',
			'0',
			tmpData.FileName);

			mysql_query(pcon, strSql);
			return JudgeSqlExecResultLocal(0,"DBS_INSERT ",pcon);
		case DBS_DELETE:
			sprintf(strSql,"DELETE FROM BAGINFO WHERE INSERTDATE <str_to_date('%s','%s')",tmpData.InsertDate,"%Y-%m-%d %H:%i:%s";
			mysql_query(pcon, strSql);
			return JudgeSqlExecResultLocal(0,"DBS_DELETE ",pcon);
	}
	return SUCESS;
}
