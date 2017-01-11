/*
 * dbbundleinfo.h
 *
 *  Created on: 2017Äê1ÔÂ9ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBBUNDLEINFO_H_
#define INCLUDE_DB_DBBUNDLEINFO_H_
#include "switch.h"
typedef struct t_Bundleinfo
{
	char seqid[11+1];
	char bundleid[24+1];
	char percode[24+1];
	char flag;
	char scantime[20+1];
	char createtime[20+1];
	char operatorid[20+1];
	char checker[20+1];
	char addmonoperator[20+1];
	char addmonchecker[20+1];
	char addmontime[20+1];
	char bankno[20+1];
	char agencyno[20+1];
	char filename[60+1];
#ifdef BFNEW
	char id[30+1];
#endif
}BUNDLEINFO, *pBundleinfo;

int DbBundleinfo(int oprType, BUNDLEINFO *record);

#endif /* INCLUDE_DB_DBBUNDLEINFO_H_ */
