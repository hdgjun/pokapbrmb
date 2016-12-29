#include "db/db.h"
#include "db/dbmoneydataatm.h"
#include "common.h"
static const char insertsql[] = "INSERT DELAYED INTO MONEYDATA_ATM (\
		PERCODE,COLTIME,MON,MONTYPE,MONVAL,MONVER,TRUEFLAG,\
		OPERDATE,IMAGEPATH,BUSINESSTYPE,\
		BANKNO,AGENCYNO,BUSINESSID) VALUES(";
int DbsMoneydataAtm(int oprType, ATMRECODE *fileRecord)
{
#if 0
	POKA_MYSQL
	FILERECORD  tmpData;

	char strSql[300+FILE_PATH_CHARNUM*21] = {0};

	memset(&tmpData, 0x00, sizeof(FILERECORD));
	memcpy(&tmpData, fileRecord, sizeof(FILERECORD));

	switch (oprType)
	{
		case DBS_INSERT:
			sprintf(strSql, "%s '%s', str_to_date('%s','%s'), '%s', '%s', '%s', '%s', '%s', '%s', %s, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s','%s','%s')", insertsql,
				tmpData.MachineSNo,
				tmpData.DateTime,//SYSDATE,	//记录时间
				"%Y-%m-%d %H:%i:%s",
				tmpData.SNo,
				tmpData.MoneyFlag,
				tmpData.Valuta,
				tmpData.Ver,
				tmpData.tfFlag,
				//"90",											//成色
				//"",												//冠字号图像
				tmpData.Operator,//:pfilename->CounterNo,	//操作员(可能导致程序终止)
				"NOW()",										//插入数据库日期
				tmpData.BundleID,							//把条码
				tmpData.PackageId,
				tmpData.Monitor,								//监督员
				tmpData.ImageFilePath,
				tmpData.BusinessType,						//业务类型
				tmpData.MonBoxID,							//钞箱ID
				tmpData.AtmID,								//ATM机ID
				tmpData.AddMonOperator,						//加钞员
				tmpData.AddMonChecker,						//加钞监管员
				//"",
				tmpData.BankNo,
				tmpData.FileName,
				tmpData.AgencyNo
				);
			 mysql_query(pcon, strSql);
			return JudgeSqlExecResultLocal(0,"DBS_INSERT ",pcon);
	}
#endif
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
					BANKNO,AGENCYNO,BUSINESSID) VALUES( '%s', \
					str_to_date('%s','%s'), '%s', '%s', '%s', \
					'%s', '%c', CURRENT_TIMESTAMP(), '%s', '%s', '%s', '%s', '%s')",tn,
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
					tmpData.agencyno,
					tmpData.businessid);
			mysql_query(pcon, strSql);

			return JudgeSqlExecResultLocal(0,"DBS_INSERT TEMP",pcon);
		case DBS_CREATE_TEMP:
			sprintf(strSql,"create temporary table %s type='heap' \
						 select * from MONEYDATA_ATM where 1=0;",tn);
			mysql_query(pcon, strSql);

			return JudgeSqlExecResultLocal(0,"DBS_CREATE_TEMP",pcon);
		case DBS_SELECT:
			sprintf(strSql,"select count(1) from %s ;",tn);
			mysql_query(pcon, strSql);
			if(mysql_errno(pcon)==1146){
				return ERROR;
			}else{
				return SUCESS;
			}
		case DBS_TRUNCATE:
			sprintf(strSql,"truncate table %s ",tn);
			mysql_query(pcon, strSql);
			return JudgeSqlExecResultLocal(0,"DBS_TRUNCATE",pcon);
	}
	return SUCESS;
}
