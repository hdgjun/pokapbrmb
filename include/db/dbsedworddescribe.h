/*
 * dbsedworddescribe.h
 *
 *  Created on: 2016Äê12ÔÂ27ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBSEDWORDDESCRIBE_H_
#define INCLUDE_DB_DBSEDWORDDESCRIBE_H_

typedef struct t_SendWordDescribe
{
	unsigned int id;
	char describe[100+1];
	char servicecode[20+1];
    unsigned int localinsert;
}SEDWORDDESCRIBE, *pSendWordDescribe;

int DbSendWordDescribe(int oprType, SEDWORDDESCRIBE *record);

#endif /* INCLUDE_DB_DBSEDWORDDESCRIBE_H_ */
