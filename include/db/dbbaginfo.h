/*
 * dbbaginfo.h
 *
 *  Created on: 2016��12��25��
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBBAGINFO_H_
#define INCLUDE_DB_DBBAGINFO_H_

//������Ϣ�ṹ��
typedef struct T_BagInfo
{
	char BagCode[20+1];					//����ǩ
	char BankId[20+1];					//���к�
	char BagDate[20+1];		//װ��ʱ��
	char BagBagId[25];					//װ��Ա
	char BagCheckId[20+1];				//װ������Ա
	char BundleCode[30+1];				//����ǩ
	char BundleDate[20+1];	//����ʱ��
	char InsertDate[20+1];
	char Writestatus;
	char Putstatus;
	char FileName[60+1];

}BAGINFO,pBagInfo;

int DbBaginfo(int oprType, BAGINFO *fileRecord);
#endif /* INCLUDE_DB_DBBAGINFO_H_ */
