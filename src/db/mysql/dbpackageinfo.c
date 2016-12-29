/*
 * dbpackageinfo.pc
 *
 *  Created on: 2016Äê12ÔÂ25ÈÕ
 *      Author: Administrator
 */
#include "db/db.h"
#include "db/dbpackageinfo.h"
#include "common.h"
int DbPackageinfo(int oprType, DBPACKAGEINFO *fileRecord)
{
	POKA_MYSQL
	DBPACKAGEINFO  tmpData;
	char strSql[300] = {0};

	memset(&tmpData, 0x00, sizeof(DBPACKAGEINFO));
	memcpy(&tmpData, fileRecord, sizeof(DBPACKAGEINFO));

	switch (oprType)
	{
		case DBS_INSERT:
			sprintf(strSql, "INSERT INTO PACKAGEINFO(\
					PACKAGEID,BUNDLEID,REMARK,OPERATORID,\
					OPERTIME,CHECKER,FILENAME,INSERTDATE) VALUES(\
					'%s', '%s', '%s', '%s', str_to_date('%s','%s'), '%s','%s',NOW())",\
						 tmpData.packageId,
						 tmpData.bundleId,
						 tmpData.remark,
						 tmpData.operatorid,
						 tmpData.opertime,
						 "%Y-%m-%d %H:%i:%s",
						 tmpData.checker,
						 tmpData.fileName
						 );
			printf("strSql:%s\n",strSql);
			mysql_query(pcon, strSql);
			return JudgeSqlExecResultLocal(0,"DBS_INSERT ",pcon);
	}
}
