/*
 * route.h
 *
 *  Created on: 2016Äê12ÔÂ27ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBROUTE_H_
#define INCLUDE_DB_DBROUTE_H_

typedef struct t_Route
{
	unsigned int id;
	char targetbank[20+1];
	char forwardbank[20+1];
	char user[20+1];
	char password[20+1];
	char localdir[100+1];
	char remotedir[100+1];
	unsigned int type;
	unsigned int servicecode;
	unsigned int lastdate;
	unsigned int starttime;
    unsigned int interval;
    char status;
    char ipaddr[20+1];
    char port[8+1];
}ROUTE, *pRoute;

int DbRoute(int oprType, ROUTE *record);

#endif /* INCLUDE_DB_DBROUTE_H_ */
