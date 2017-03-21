/*
 * dbkunfiles.h
 *
 *  Created on: 2017Äê3ÔÂ17ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBKUNFILES_H_
#define INCLUDE_DB_DBKUNFILES_H_

typedef struct T_Kunfiles
{
	char bundleCode[30+1];
	char type[8+1];
	char path[256+1];
	char name[100+1];
	char insertdate[20+1];
}TKUNFILES;

int DbKunFiles(int oprType, TKUNFILES *data);
#endif /* INCLUDE_DB_DBKUNFILES_H_ */
