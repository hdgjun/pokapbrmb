/*
 * dbskjl.h
 *
 *  Created on: 2017Äê1ÔÂ13ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBSKJL_H_
#define INCLUDE_DB_DBSKJL_H_

#define BUSINESS_DATE_LEN 19
#define USER_ID_LEN       8
#define ACCOUNT_NO_LEN    25
#define PERCODE_LEN       24

typedef struct t_Dbskjl
{
	unsigned int id;
	char bankno[20+1];
	char notno[20+1];
	char userid[8+1];
	char percode[30+1];
	char businessdate[19+1];
	char accountno[30+1];
    char insertdate[20+1];
}DBSKJL, *pSkjl;

int DbSKJL(int oprType, DBSKJL *record);
#endif /* INCLUDE_DB_DBSKJL_H_ */
