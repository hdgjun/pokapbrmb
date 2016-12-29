/*
 * dbpackageinfo.h
 *
 *  Created on: 2016Äê12ÔÂ25ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBPACKAGEINFO_H_
#define INCLUDE_DB_DBPACKAGEINFO_H_

typedef struct t_db_packageinfo
{
	char packageId[24+1];
	char bundleId[24+1];
	char operatorid[20+1];
	char opertime[20+1];
	char checker[20+1];
	char insertdate[20+1];
	char fileName[60+1];
	char remark[512+1];
}DBPACKAGEINFO,*pDbPackageinfo;

int DbPackageinfo(int oprType, DBPACKAGEINFO *fileRecord);
#endif /* INCLUDE_DB_DBPACKAGEINFO_H_ */
