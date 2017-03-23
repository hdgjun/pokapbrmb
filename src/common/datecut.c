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
#include "filepath.h"

#include "db/dbbaginfo.h"
#include "db/dbbundleinfo.h"
#include "db/dbmonboxaddmon.h"
#include "db/dbmoneydata.h"
#include "db/dbpackageinfo.h"
#include "db/dbskjl.h"
#include "db/dbmoneydataatm.h"

static DATECUT g_dc;

/*delete db old data*/
static void cleanDBOldData()
{
	int date = GetDateInterval(0-g_param.DbSaveDay);
	vLog("delete db data[%d]",date);
	BAGINFO bag = {0};
	sprintf(bag.InsertDate,"%d",date);
	DbBaginfo(DBS_DELETE,&bag);
	DbsCommit();

	BUNDLEINFO bundle = {0};
	sprintf(bundle.addmontime,"%d",date);
	DbBundleinfo(DBS_DELETE,&bundle);
	DbsCommit();

	MONBOXADDMON monbox = {0};
	sprintf(monbox.opertime,"%d",date);
	DbMonboxaddmon(DBS_DELETE,&monbox);
	DbsCommit();

	DBPACKAGEINFO package = {0};
	sprintf(package.insertdate,"%d",date);
	DbPackageinfo(DBS_DELETE,&package);
	DbsCommit();

	DBSKJL skjl = {0};
	sprintf(skjl.insertdate,"%d",date);
	DbSKJL(DBS_DELETE,&skjl);
	DbsCommit();

	ATMRECODE atmreco = {0};
	sprintf(atmreco.operdate,"%d",date);
	DbsMoneydataAtm(DBS_DELETE,&atmreco);
	DbsCommit();

	MONEYDATAR monar = {0};
	sprintf(monar.operdate,"%d",date);
	DbsDoubtMoneydata(DBS_DELETE,&monar);
	DbsCommit();

	DbsMoneydata(DBS_DELETE,&monar);
	DbsCommit();

	vLog("delete db data finished");
}

int init_date_cut()
{
	if(DbDateCut(DBS_SELECT, &(g_dc))!=SUCESS)
	{
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
		DbsCommit();
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
#ifdef DEBUG
	vLog("cut status[%d]",g_dc.status);
#endif
	if( g_dc.status  == DATE_START){
		if(t>g_dc.endtime||t<g_dc.cuttime){
			g_dc.status = DATE_END;
			g_dc.lastdate = g_dc.curdate;
			g_dc.curdate = d;
			DbDateCut(DBS_UPDATE, &g_dc);
			DbsCommit();
			vLog("stop data cut");
		}
	}else if( g_dc.status  == DATE_END){
		if(t>g_dc.cuttime&&t<g_dc.endtime){
			g_dc.status = DATE_START;
			DbDateCut(DBS_UPDATE, &(g_dc));
			DbsCommit();
			vLog("start data cut");
			cleanDBOldData();
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
		sleep(10 * SLEEPTIME);
		SLEEPTIME++;
		SLEEPTIME = SLEEPTIME % 180;
	}

	init_date_cut();
	while(1){
		update_date_cup();
		sleep(60);
	}
}
