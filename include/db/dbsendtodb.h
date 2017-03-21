/*
 * dbsendtodb.h
 *
 *  Created on: 2017Äê1ÔÂ17ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBSENDTODB_H_
#define INCLUDE_DB_DBSENDTODB_H_

typedef struct T_SendToPb
{
	char id[30+1];
	char bundleid[20+1];
	char ftype[5+1];
	char stats;
	char insertdate[20+1];
}SENDTOPB,pSendToPb;

int DbSendToPb(int oprType, SENDTOPB *re);

#endif /* INCLUDE_DB_DBSENDTODB_H_ */
