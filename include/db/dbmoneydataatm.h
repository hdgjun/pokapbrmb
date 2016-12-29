#ifndef _DB_MONEYDATA_ATM_H_
#define _DB_MONEYDATA_ATM_H_

typedef struct t_AtmRecord
{
	char id[100];
	char percode[24+1];
	char coltime[20+1];
	char mon[20+1];
	char montype[3+1];
	char monval[8+1];
	char monver[4+1];
	char trueflag;
	char quanlity[2+1];
	char operdate[20+1];
	char imagepath[256+1];
	char businesstype[4+1];
	char bankno[20+1];
	char agencyno[20+1];
	char businessid[30+1];
	char reserve1[50+1];
	char reserve2[40+1];
}ATMRECODE, *pAtmRecode;

int DbsMoneydataAtm(int oprType, ATMRECODE *fileRecord);
int TmpDbsMoneydataAtm(int oprType,const char *tn,ATMRECODE *fileRecord);
int FromTmpToAtm(const char *tn,const char *id);
#endif
