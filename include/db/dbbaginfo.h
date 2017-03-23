/*
 * dbbaginfo.h
 *
 *  Created on: 2016年12月25日
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBBAGINFO_H_
#define INCLUDE_DB_DBBAGINFO_H_

//袋捆信息结构体
typedef struct T_BagInfo
{
	char BagCode[20+1];					//袋标签
	char BankId[20+1];					//银行号
	char BagDate[20+1];		//装袋时间
	char BagBagId[25];					//装袋员
	char BagCheckId[20+1];				//装袋复核员
	char BundleCode[30+1];				//捆标签
	char BundleDate[20+1];	//打捆时间
	char InsertDate[20+1];
	char Writestatus;
	char Putstatus;
	char FileName[60+1];

}BAGINFO,pBagInfo;

int DbBaginfo(int oprType, BAGINFO *fileRecord);
#endif /* INCLUDE_DB_DBBAGINFO_H_ */
