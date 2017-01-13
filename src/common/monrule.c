/*
 * monrule.c
 *
 *  Created on: 2017Äê1ÔÂ12ÈÕ
 *      Author: Administrator
 */

#include "common.h"
#include "db/db.h"
#include "monrule.h"

static int cur_rule = -1;


void updateMonrule()
{
	int next = (cur_rule+1)%2;

	MONRULEARRY *arr;

	arr = &g_mon_rule[next];

	DbMonRule(DBS_SELECT, arr);
	cur_rule = next;
}
MONRULEARRY * getMonrule()
{
	if(cur_rule<0||cur_rule>=2)return NULL;

	return &g_mon_rule[cur_rule];
}
