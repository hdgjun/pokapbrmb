/*
 * dbperinfo.h
 *
 *  Created on: 2016Äê12ÔÂ26ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBPERINFO_H_
#define INCLUDE_DB_DBPERINFO_H_

typedef struct t_Perinfo
{
	char percode[22+1];
	char pertype[2+1];
	char bankno[20+1];
	char agencyno[20+1];
	char acctaddr[120+1];
	char brand[50+1];
	char model[100+1];
	char quality[100+1];
	char status;
	char usestatus;
	char moduser[60+1];
	char terminalno[30+1];
}PERINFO, *pPerinfo;

int DbPerinfo(int oprType, PERINFO *record);


#endif /* INCLUDE_DB_DBPERINFO_H_ */
