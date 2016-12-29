/*
 * dbrouterule.h
 *
 *  Created on: 2016��12��28��
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBROUTERULE_H_
#define INCLUDE_DB_DBROUTERULE_H_

#define MAX_RULE 30

typedef struct t_RouteRule
{
	unsigned int id;
	char fileextend[10+1];
	unsigned int routeid;
}ROUTERULE, *pRouteRule;


int DbRouteRule(ROUTERULE *record,int *size,int id);

#endif /* INCLUDE_DB_DBROUTERULE_H_ */
