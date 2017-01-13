/*
 * dbmonrule.h
 *
 *  Created on: 2017年1月12日
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBMONRULE_H_
#define INCLUDE_DB_DBMONRULE_H_

#define MAX_RULE 100
//冠字号黑名单
typedef struct t_MonRule
{
	char MonHead[12+1];			//可疑冠字号
	char MonVer[20+1];			//版别
	char MonVal[5+1];			//币值以角为单位
}MONRULE,*pMonRule;

typedef struct t_MonRule_array
{
	MONRULE data[MAX_RULE];
	int size;
}MONRULEARRY;

#define  DOUBTSNOFLAG_CHAR 				'9'//黑名单冠字号标志
#define	 MATCH_CHAR						'*'

int DbMonRule(int oprType, MONRULEARRY *re);

#endif /* INCLUDE_DB_DBMONRULE_H_ */
