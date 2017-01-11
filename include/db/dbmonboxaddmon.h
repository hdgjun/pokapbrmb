/*
 * dbmonboxaddmon.h
 *
 *  Created on: 2017Äê1ÔÂ9ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBMONBOXADDMON_H_
#define INCLUDE_DB_DBMONBOXADDMON_H_
#include "switch.h"
typedef struct t_Monboxaddmon
{
	char scanid[24+1];
	char flag;
	char monboxid[11+1];
	char remark[512+1];
	char opertime[20+1];
	char operatorid[20+1];
	char checker[20+1];
	char insertdate[20+1];
#ifdef BFNEW
	char id[30+1];
#endif
	char filename[60+1];
}MONBOXADDMON, *pMonboxaddmon;

int DbMonboxaddmon(int oprType, MONBOXADDMON *record);

#endif /* INCLUDE_DB_DBMONBOXADDMON_H_ */
