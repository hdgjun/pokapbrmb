#include "db/db.h"
#include "common.h"
#include "db/dbmoneydata.h"

static const char insertsql[] = "INSERT DELAYED INTO MONEYDATA(\
		PERCODE,COLTIME,MON,MONTYPE,MONVAL,MONVER,TRUEFLAG,\
		OPERATORID,OPERDATE,BUNDLEID,PACKAGEID,CHECKER,IMAGEPATH,\
		BUSINESSTYPE,MONBOXID,ATMID,ADDMONOPERATOR,ADDMONCHECKER,\
		BANKNO,FILENAME,AGENCYNO) VALUES(";
static const char insertdoubtsql[] = "INSERT DELAYED INTO DOUBTMONEYDATA(\
		PERCODE,COLTIME,MON,MONTYPE,MONVAL,MONVER,TRUEFLAG,\
		OPERATORID,OPERDATE,BUNDLEID,PACKAGEID,CHECKER,IMAGEPATH,\
		BUSINESSTYPE,MONBOXID,ATMID,ADDMONOPERATOR,ADDMONCHECKER,\
		BANKNO,FILENAME,AGENCYNO) VALUES(";
int DbsMoneydata(int oprType, MONEYDATAR *fileRecord)
{
	POKA_MYSQL

	MONEYDATAR  tmpData;
	char strSql[300+FILE_PATH_CHARNUM*21] = {0};

	memset(&tmpData, 0x00, sizeof(MONEYDATAR));
	memcpy(&tmpData, fileRecord, sizeof(MONEYDATAR));

	switch (oprType)
	{
		case DBS_INSERT:
			sprintf(strSql, "%s '%s', str_to_date('%s','%s'), '%s', '%s', '%s', '%s', '%c', '%s', %s, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s','%s','%s')", insertsql,
				tmpData.percode,
				tmpData.coltime,//SYSDATE,	//��¼ʱ��
				"%Y-%m-%d %H:%i:%s",
				tmpData.mon,
				tmpData.montype,
				tmpData.monval,
				tmpData.monver,
				tmpData.trueflag,
				//"90",											//��ɫ
				//"",												//���ֺ�ͼ��
				tmpData.operatorid,//:pfilename->CounterNo,	//����Ա(���ܵ��³�����ֹ)
				"NOW()",										//�������ݿ�����
				tmpData.bundleid,							//������
				tmpData.packageid,
				tmpData.checker,								//�ලԱ
				tmpData.imagepath,
				tmpData.businesstype,						//ҵ������
				tmpData.monboxid,							//����ID
				tmpData.atmid,								//ATM��ID
				tmpData.addmonoperator,						//�ӳ�Ա
				tmpData.addmonchecker,						//�ӳ����Ա
				//"",
				tmpData.bankno,
				tmpData.filename,
				tmpData.agencyno
				);
			 mysql_query(pcon, strSql);
			return JudgeSqlExecResultLocal(0,"DBS_INSERT ",pcon);
	}
   return SUCESS;
}

int DbsDoubtMoneydata(int oprType,MONEYDATAR *fileRecord)
{
	POKA_MYSQL

	MONEYDATAR  tmpData;
	char strSql[300+FILE_PATH_CHARNUM*21] = {0};

	memset(&tmpData, 0x00, sizeof(MONEYDATAR));
	memcpy(&tmpData, fileRecord, sizeof(MONEYDATAR));

	switch (oprType)
	{
		case DBS_INSERT:
			sprintf(strSql, "%s '%s', str_to_date('%s','%s'), '%s', '%s', '%s', '%s', '%c', '%s', %s, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s','%s','%s')", insertdoubtsql,
				tmpData.percode,
				tmpData.coltime,//SYSDATE,	//��¼ʱ��
				"%Y-%m-%d %H:%i:%s",
				tmpData.mon,
				tmpData.montype,
				tmpData.monval,
				tmpData.monver,
				tmpData.trueflag,
				//"90",											//��ɫ
				//"",												//���ֺ�ͼ��
				tmpData.operatorid,//:pfilename->CounterNo,	//����Ա(���ܵ��³�����ֹ)
				"NOW()",										//�������ݿ�����
				tmpData.bundleid,							//������
				tmpData.packageid,
				tmpData.checker,								//�ලԱ
				tmpData.imagepath,
				tmpData.businesstype,						//ҵ������
				tmpData.monboxid,							//����ID
				tmpData.atmid,								//ATM��ID
				tmpData.addmonoperator,						//�ӳ�Ա
				tmpData.addmonchecker,						//�ӳ����Ա
				//"",
				tmpData.bankno,
				tmpData.filename,
				tmpData.agencyno
				);
			 mysql_query(pcon, strSql);
			return JudgeSqlExecResultLocal(0,"DBS_INSERT ",pcon);
	}
   return SUCESS;
}
