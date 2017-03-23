#include "db/db.h"
#include "db/dbmoneydataatm.h"
#include "common.h"
#include "pokalog.h"

int DbsMoneydataAtm(int oprType, ATMRECODE *fileRecord)
{

	POKA_MYSQL
	ATMRECODE  tmpData;

	char strSql[300+FILE_PATH_CHARNUM*21] = {0};

	if(fileRecord)
	{
		memset(&tmpData, 0x00, sizeof(ATMRECODE));
		memcpy(&tmpData, fileRecord, sizeof(ATMRECODE));
	}
	switch (oprType)
	{
		case sprintf(strSql,"DELETE FROM MONEYDATA_ATM WHERE OPERDATE <str_to_date('%s','%s')",tmpData.operdate,"%Y-%m-%d %H:%i:%s";
		mysql_query(pcon, strSql);
		return JudgeSqlExecResultLocal(0,"DBS_DELETE ",pcon);
	}

   return SUCESS;
}

int FromTmpToAtm(const char *tn,const char *id)
{
	POKA_MYSQL


	char strSql[300+FILE_PATH_CHARNUM*21] = {0};

	sprintf(strSql,"insert into MONEYDATA_ATM(PERCODE,\
		COLTIME,\
		MON,\
		MONTYPE,\
		MONVAL,\
		MONVER,\
		TRUEFLAG,\
		QUANLITY,\
        IMAGEPATH,\
		BUSINESSTYPE,\
		BANKNO,\
		AGENCYNO,\
		BUSINESSID,\
		OPERDATE,\
		RESERVE1,\
		RESERVE2 )\
		(select PERCODE,COLTIME,MON,MONTYPE,MONVAL,MONVER,TRUEFLAG,QUANLITY,\
					IMAGEPATH,BUSINESSTYPE,BANKNO,AGENCYNO,B.ID AS BUSINESSID,B.OPERDATE AS OPERDATE,RESERVE1,RESERVE2 \
			from ( %s A , (select '%s' as id,CURRENT_TIMESTAMP() as OPERDATE from DUAL) B))",tn,id);
	mysql_query(pcon, strSql);

	return JudgeSqlExecResultLocal(0,"DBS_INSERT TEMP",pcon);
}

int TmpDbsMoneydataAtm(int oprType,const char *tn,ATMRECODE *fileRecord)
{
	POKA_MYSQL

	ATMRECODE  tmpData;

	char strSql[300+FILE_PATH_CHARNUM*21] = {0};

	if(fileRecord){
		memset(&tmpData, 0x00, sizeof(ATMRECODE));
		memcpy(&tmpData, fileRecord, sizeof(ATMRECODE));
	}
	switch (oprType)
	{
		case DBS_INSERT:
			sprintf(strSql,"INSERT  INTO %s (\
					PERCODE,COLTIME,MON,MONTYPE,MONVAL,MONVER,TRUEFLAG,\
					OPERDATE,IMAGEPATH,BUSINESSTYPE,\
					BANKNO,AGENCYNO) VALUES( '%s', \
					str_to_date('%s','%s'), '%s', '%s', '%s', \
					'%s', '%c', CURRENT_TIMESTAMP(), '%s', '%s', '%s', '%s')",tn,
					tmpData.percode,
					tmpData.coltime,
					"%Y%m%d%H%i%s",
					tmpData.mon,
					tmpData.montype,
					tmpData.monval,
					tmpData.monver,
					tmpData.trueflag,
					tmpData.imagepath,
					tmpData.businesstype,
					tmpData.bankno,
					tmpData.agencyno);
			mysql_query(pcon, strSql);
			vLog("sql:[%s]",strSql);
			return JudgeSqlExecResultLocal(0,"DBS_INSERT TEMP",pcon);
		case DBS_CREATE_TEMP:
			sprintf(strSql,"create temporary table %s  \
						 select * from MONEYDATA_ATM where 1=0;",tn);
			mysql_query(pcon, strSql);

			return JudgeSqlExecResultLocal(0,"DBS_CREATE_TEMP",pcon);
		case DBS_SELECT:
			sprintf(strSql,"select count(1) from %s ;",tn);
			mysql_query(pcon, strSql);
			if(mysql_errno(pcon)==1146){
				return ERROR;
			}else{
				ctx->result =  mysql_store_result(pcon);
				if(ctx->result)
				mysql_free_result(ctx->result);
				return SUCESS;
			}
		case DBS_TRUNCATE:
			sprintf(strSql,"truncate table %s ",tn);
			mysql_query(pcon, strSql);
			return JudgeSqlExecResultLocal(0,"DBS_TRUNCATE",pcon);
	}
	return SUCESS;
}
