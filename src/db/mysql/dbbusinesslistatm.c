/*
 * dbbusinesslistatm.c
 *
 *  Created on: 2016Äê12ÔÂ26ÈÕ
 *      Author: Administrator
 */
#include "db/db.h"
#include "db/dbbusinesslistatm.h"
#include "common.h"
int DbBusinessListAtm(int oprType, ATMBUSINESSLIST *record)
{
	POKA_MYSQL

	ATMBUSINESSLIST  tmpData;

	char strSql[300+FILE_PATH_CHARNUM*21] = {0};

	if(record){
		memset(&tmpData, 0x00, sizeof(ATMBUSINESSLIST));
		memcpy(&tmpData, record, sizeof(ATMBUSINESSLIST));
	}
	int iRet;
	MYSQL_ROW row;
	unsigned long ulrow = 0;
	switch (oprType)
	{
		case DBS_INSERT:
			sprintf(strSql,"INSERT INTO BUSINESSLIST_ATM(BANKNO,NETNO,PERCODE,\
					ACCOUNTNO,BUSINESSDATE,BUSINESSID,MONEYTOTAL,\
					INOROUT,COREID,REMARK,INSERTDATE) VALUES('%s', '%s','%s','%s',\
					str_to_date('%s','%s'),'%s','%s','%c','%s','%s',%s)",
					tmpData.bankno,
					tmpData.netno,
					tmpData.percode,
					tmpData.accountno,
					tmpData.businessdate,
					"%Y%m%d%H%i%s",
					tmpData.businessid,
					tmpData.moneytotal,
					tmpData.inorout,
					tmpData.coreid,
					tmpData.remark,
					"CURRENT_TIMESTAMP()");
			mysql_query(pcon, strSql);
			return JudgeSqlExecResultLocal(0,"DBS_INSERT BUSINESSLIST_ATM",pcon);

		case DBS_SEL_ONE_ROW:
			sprintf(strSql,"SELECT ID FROM BUSINESSLIST_ATM WHERE NETNO=%s and\
							ACCOUNTNO='%s' and BUSINESSDATE=str_to_date('%s','%s') and\
					PERCODE='%s' and BUSINESSID='%s'",tmpData.netno,tmpData.accountno,tmpData.businessdate,
					"%Y%m%d%H%i%s",tmpData.percode,tmpData.businessid);
			mysql_query(pcon,strSql);
			iRet = JudgeSqlExecResultLocal(0,"DBS_SEL_ONE_ROW ",pcon);
			printf(strSql);
			if(iRet != SUCESS)return iRet;

			ctx->result =  mysql_store_result(pcon);
			ulrow = (unsigned long)mysql_num_rows(ctx->result);
			if(ctx->result&&ulrow>0)
			{
				row = mysql_fetch_row(ctx->result);
				if(row[0]!=NULL)memcpy(tmpData.id,row[0],strlen(row[0]));
				mysql_free_result(ctx->result);
				memcpy(record,&tmpData,sizeof(ATMBUSINESSLIST));
				return SUCESS;
			}
			return ERROR;
		case DBS_DELETE:
			sprintf(strSql,"DELETE FROM BUSINESSLIST_ATM WHERE NETNO=%s and\
							ACCOUNTNO='%s' and BUSINESSDATE=str_to_date('%s','%s') and\
					PERCODE='%s' and BUSINESSID='%s'",tmpData.netno,tmpData.accountno,tmpData.businessdate,
					"%Y%m%d%H%i%s",tmpData.percode,tmpData.businessid);
			mysql_query(pcon,strSql);
			return JudgeSqlExecResultLocal(0,"DBS_DELETE  BUSINESSLIST_ATM",pcon);
		case DBS_SELECT1:
			sprintf(strSql,"SELECT ID FROM BUSINESSLIST_ATM WHERE NETNO=%s and\
					ACCOUNTNO='%s' and BUSINESSDATE=str_to_date('%s','%s') and\
					PERCODE='%s' and BUSINESSID='%s'",tmpData.netno,tmpData.accountno,tmpData.businessdate,
					"%Y%m%d%H%i%s",tmpData.percode,tmpData.businessid);
			mysql_query(pcon,strSql);
			iRet = JudgeSqlExecResultLocal(0,"DBS_SELECT1 ",pcon);
			if(iRet != SUCESS)
			{
				return WARING;
			}
			ctx->result =  mysql_store_result(pcon);
			ulrow = (unsigned long)mysql_num_rows(ctx->result);
			if(ctx->result)mysql_free_result(ctx->result);
			if(ulrow>=1){
				 return ERROR;
			 }else{
				 return SUCESS;
			 }
	}
	return SUCESS;
}
