/*
 * dbbankno.h
 *
 *  Created on: 2016Äê12ÔÂ27ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBBANKINFO_H_
#define INCLUDE_DB_DBBANKINFO_H_

typedef struct t_Bankinfo
{
	char bankno[20+1];
	char bankname[120+1];
	char ipaddr[20+1];
	char ipport[8+1];
	char address[200+1];
	char telphone[20+1];
	char remark[512+1];
}BANKINFO, *pBankinfo;

int DbBankinfo(int oprType, BANKINFO *record);

#endif /* INCLUDE_DB_DBBANKINFO_H_ */
