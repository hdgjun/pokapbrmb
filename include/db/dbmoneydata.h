#ifndef _DB_MONEYDATA_H_
#define _DB_MONEYDATA_H_

typedef struct t_Moneydata
{
	char percode[24+1];
	char coltime[20+1];
	char mon[12+1];
	char montype[3+1];
	char monval[3+1];
	char monver[4+1];
	char trueflag;
	char quanlity[2+1];
	char imagesno[500+1];
	char operatorid[20+1];
	char operdate[20+1];
	char bundleid[24+1];
	char checker[20+1];
	char imagepath[256+1];
	char businesstype[4+1];
	char monboxid[24+1];
	char atmid[24+1];
	char addmonoperator[20+1];
	char addmonchecker[20+1];
	char testimagechar[500+1];
	unsigned int seqid;
	char bankno[20+1];
	char agencyno[20+1];
	char filename[60+1];
	char packageid[24+1];
}MONEYDATAR, *pMoneydatar;


int DbsMoneydata(int oprType,MONEYDATAR *fileRecord);

#endif
