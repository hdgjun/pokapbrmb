/*
 * dbmonrule.h
 *
 *  Created on: 2017��1��12��
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBMONRULE_H_
#define INCLUDE_DB_DBMONRULE_H_

#define MAX_RULE 100
//���ֺź�����
typedef struct t_MonRule
{
	char MonHead[12+1];			//���ɹ��ֺ�
	char MonVer[20+1];			//���
	char MonVal[5+1];			//��ֵ�Խ�Ϊ��λ
}MONRULE,*pMonRule;

typedef struct t_MonRule_array
{
	MONRULE data[MAX_RULE];
	int size;
}MONRULEARRY;

#define  DOUBTSNOFLAG_CHAR 				'9'//���������ֺű�־
#define	 MATCH_CHAR						'*'

int DbMonRule(int oprType, MONRULEARRY *re);

#endif /* INCLUDE_DB_DBMONRULE_H_ */
