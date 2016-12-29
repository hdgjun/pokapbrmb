/*
 * dbrouterule.c
 *
 *  Created on: 2016Äê12ÔÂ28ÈÕ
 *      Author: Administrator
 */

#include "db/db.h"
#include "db/dbrouterule.h"
#include "common.h"
#include "pokalog.h"

int DbRouteRule(ROUTERULE *record,int *size,int id)
{
	POKA_MYSQL

	MYSQL_ROW row;
	unsigned long ulrow;
	char strSql[300+FILE_PATH_CHARNUM*21] = {0};

	sprintf(strSql,"SELECT FILEEXTEND FROM ROUTERULE WHERE ROUTEID=%d",id);
	mysql_query(pcon,strSql);
	ctx->result =  mysql_store_result(pcon);
	ulrow = (unsigned long)mysql_num_rows(ctx->result);
	if(ulrow <=0){
		*size = 0;
		if(ctx->result)
		mysql_free_result(ctx->result);
		return SUCESS;
	}else{
		*size = (int)ulrow;
		int i = 0;
		while((row = mysql_fetch_row(ctx->result)))
		{
			vLog("*********************[%d:%d]******************************",i,*size);
			if(row[0]!=NULL)memcpy((record+i)->fileextend,row[0],strlen(row[0]));
			vLog("*********************fileextend[%s]******************************",(record+i)->fileextend);
			i++;
		}
		if(ctx->result)
			mysql_free_result(ctx->result);
	}
	return SUCESS;
}
