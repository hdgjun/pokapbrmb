/*
 * dbsearchfsnbk.h
 *
 *  Created on: 2017Äê3ÔÂ17ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBSEARCHFSNBK_H_
#define INCLUDE_DB_DBSEARCHFSNBK_H_

typedef struct T_SearchFsnBk
{
	int  id;
	char bundleCode[30+1];
	char orderId[30+1];
	char type[8+1];
}TSEARCHFSNBK;

int DbCheakFsnBk(int oprType,TSEARCHFSNBK *data,int *result);
int DbSearchFsnBk(int oprType, TSEARCHFSNBK *data);
#endif /* INCLUDE_DB_DBSEARCHFSNBK_H_ */
