/*
 * dbbundleinfo.c
 *
 *  Created on: 2017Äê1ÔÂ9ÈÕ
 *      Author: Administrator
 */

#include "db/db.h"
#include "db/dbbundleinfo.h"
#include "common.h"
#include "switch.h"
int DbBundleinfo(int oprType, BUNDLEINFO *record)
{
	POKA_MYSQL

	char strSql[1024] = {0};

	BUNDLEINFO  tmpData;
	if(record)
	{
		memset(&tmpData, 0x00, sizeof(BUNDLEINFO));
		memcpy(&tmpData, record, sizeof(BUNDLEINFO));
	}

	switch (oprType)
	{
		case DBS_INSERT:
#ifdef BFNEW
			sprintf(strSql,"INSERT INTO BUNDLEINFO(BUNDLEID,PERCODE,SCANTIME,CREATETIME,OPERATORID,CHECKER,ADDMONOPERATOR,ADDMONCHECKER,ADDMONTIME,FLAG,BANKNO,AGENCYNO,FILENAME,ID) \
			VALUES('%s', '%s' , str_to_date('%s','%s'), NOW(),'%s','%s','%s','%s', str_to_date('%s','%s'),'%c','%s','%s','%s','%s')",
			tmpData.bundleid,
			tmpData.percode,
			tmpData.scantime,
			"%Y-%m-%d %H:%i:%s",
			"",
			"",
			tmpData.addmonoperator,
			tmpData.addmonchecker,
			tmpData.addmontime,
			"%Y-%m-%d %H:%i:%s",
			tmpData.flag,
			tmpData.bankno,
			tmpData.agencyno,
			tmpData.filename,
			tmpData.id
			);
#else
			sprintf(strSql,"INSERT INTO BUNDLEINFO(BUNDLEID,PERCODE,SCANTIME,CREATETIME,OPERATORID,CHECKER,ADDMONOPERATOR,ADDMONCHECKER,ADDMONTIME,FLAG,BANKNO,AGENCYNO,FILENAME) \
						VALUES('%s', '%s' , str_to_date('%s','%s'), NOW(),'%s','%s','%s','%s', str_to_date('%s','%s'),'%c','%s','%s','%s')",
						tmpData.bundleid,
						tmpData.percode,
						tmpData.scantime,
						"%Y-%m-%d %H:%i:%s",
						"",
						"",
						tmpData.addmonoperator,
						tmpData.addmonchecker,
						tmpData.addmontime,
						"%Y-%m-%d %H:%i:%s",
						tmpData.flag,
						tmpData.bankno,
						tmpData.agencyno,
						tmpData.filename
						);
#endif
			mysql_query(pcon, strSql);
			return JudgeSqlExecResultLocal(0,"DBS_INSERT ",pcon);
	}
	return SUCESS;
}
