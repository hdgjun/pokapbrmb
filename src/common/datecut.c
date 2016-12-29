#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "common.h"
#include "datecut.h"
#include "switch.h"
#include "db/db.h"
#include "db/dbdatecut.h"
#include "pokacfg.h"
#include "pokalog.h"

static DATECUT g_dc;
int init_date_cut()
{
	if(DbDateCut(DBS_SELECT, &(g_dc))!=SUCESS){
		char tem[10] = {0};
		memset(&(g_dc),0x00,sizeof(DATECUT));
		g_dc.lastdate = GetDateInt();
		g_dc.curdate = GetDateInt();
		sprintf(tem,"%s",DEF_DATE_CUT_TIME);
		g_dc.cuttime = atoi(tem);
		sprintf(tem,"%s",DEF_DATE_CUT_END);
		g_dc.endtime = atoi(tem);
		int i = GetTimeInt();
		if(i>g_dc.cuttime&&i<g_dc.endtime){
			g_dc.status = DATE_START;
		}else{
			g_dc.status = DATE_END;
		}
		DbDateCut(DBS_INSERT, &(g_dc));
		DbDateCut(DBS_SELECT, &(g_dc));
	}
	return SUCESS;
}

void destroy_date_cut()
{

}
int is_date_cut()
{
	return  g_dc.status == DATE_START?SUCESS:ERROR;
}
int update_date_cup()
{
	int t = GetTimeInt();
	int d = GetDateInt();

	if( g_dc.status  == DATE_START){
		if(t>g_dc.endtime||t<g_dc.cuttime){
			g_dc.status = DATE_END;
			g_dc.lastdate = g_dc.curdate;
			g_dc.curdate = d;
			DbDateCut(DBS_UPDATE, &g_dc);
			vLog("start data cut");
		}
	}else{
		if(t>g_dc.cuttime&&t<g_dc.endtime){
			g_dc.status = DATE_START;
			DbDateCut(DBS_UPDATE, &(g_dc));
			vLog("stop data cut");
		}
	}

	return SUCESS;
}

void *date_cut_thread()
{
	int SLEEPTIME = 1;
	while (1)
	{
		if (ThreadConnectDB() == SUCESS) {
			break;
		}
		psleep(10 * SLEEPTIME);
		SLEEPTIME++;
		SLEEPTIME = SLEEPTIME % 180;
	}

	init_date_cut();
	while(1){
		update_date_cup();
		sleep(60*10);
	}
}
