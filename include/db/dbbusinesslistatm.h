/*
 * businesslistatm.h
 *
 *  Created on: 2016Äê12ÔÂ26ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBBUSINESSLISTATM_H_
#define INCLUDE_DB_DBBUSINESSLISTATM_H_

typedef struct t_AtmBusinessRecord
{
	char id[100];
	char bankno[20+1];
	char netno[20+1];
	char percode[30+1];
    char accountno[30+1];
    char businessdate[20+1];
    char businessid[50+1];
    char moneytotal[21+1];
    char inorout;
    char coreid[30+1];
    char remark[200+1];
    char inserdate[20+1];
}ATMBUSINESSLIST, *pAtmBusinessList;

int DbBusinessListAtm(int oprType, ATMBUSINESSLIST *record);
#endif /* INCLUDE_DB_DBBUSINESSLISTATM_H_ */
