/*
 * dbdatecut.h
 *
 *  Created on: 2016Äê12ÔÂ26ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBDATECUT_H_
#define INCLUDE_DB_DBDATECUT_H_

typedef struct T_DateCut
{
	unsigned int id;
	unsigned int lastdate;
	unsigned int curdate;
	unsigned int status;
	unsigned int cuttime;
	unsigned int endtime;
}DATECUT,pDateCut;

int DbDateCut(int oprType, DATECUT *re);
#endif /* INCLUDE_DB_DBDATECUT_H_ */
