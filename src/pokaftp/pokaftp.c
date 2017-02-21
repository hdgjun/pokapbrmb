#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "pokaftp.h"
#include "allcommon.h"
#include "pokalog.h"
#include "pokafile.h"
#include "switch.h"
#include "cJSON.h"
#include "db/dbroute.h"

#define MAX_ARR_SIZE 100

extern Param g_param;

static int init();
static int initlog();
static int initcfg();
static int initpath();
static int initThread();
static void CreateVerText();

static pthread_t taskfile;
static pthread_t sendfile;
static pthread_t clearFile;

static void *sendThread();
static void *SendTaskThread(void *sp);
static int Start_ftp_service(ROUTE *route);
static int Stop_ftp_service(ROUTE *route);

typedef struct Route_arr
{
	ROUTE *arr[MAX_ARR_SIZE];
	int len;
}ROTEARR;

ROTEARR g_arr;

#define ADD_ROUTE(x)    g_arr.arr[g_arr.len++]=x

#define GET_ROUTE(x)    ((x)>(MAX_ARR_SIZE)||(x)<0)?NULL:(g_arr.arr[x])\

#define CLEAN_ROUTE     int i;\
for(i=0;i<g_arr.len;i++)\
{\
free(g_arr.arr[i]);\
}\


extern pthread_attr_t attr;

int main(int argc, char **argv)
{

	if(JudgeProgramExist(1,APP_NAME) != SUCESS)
	{
		exit(0);
	}

	if(init() != SUCESS)
	{
		printf("init %s error,exit system!",APP_NAME);
		exit(-1);
	}

	int iRet,i;

	vLog("*****************************START**************************");
    i = 0;
	while(1)
	{

		/*根据路由表转发文件线程*/
		if (ESRCH == test_pthread(sendfile))
		{
			pthread_create(&sendfile, &attr, sendThread,NULL);
		}

		/*清理旧文件*/
		if (ESRCH == test_pthread(clearFile))
		{
			pthread_create(&clearFile, &attr, CleanFileThread,NULL);
		}

		sleep(g_param.SleepTime*60);
	}

	return 0;

}

static void Destory()
{
	printf("Signal deal!\n");
	pthread_attr_destroy(&attr);
	printf("Signal was stop by ctrl+c!\n");
	exit(0);
}
static int init()
{
	printf("STARTING POKAFTP!\n");

	if (initlog() != SUCESS) {
		printf("init log error!\n");
		return ERROR;
	}

	if(initcfg() != SUCESS){
		vLog("init cfg error\n");
		return ERROR;
	}

	vLog("init cfg ok!");

    if(initpath() != SUCESS){
    	vLog("init path error");
    	return ERROR;
    }
    vLog("init path ok!");

    if(initThread() != SUCESS){
    	vLog("init Thread error");
    	return ERROR;
    }
    vLog("init Thread ok!");

    signal(SIGINT,Destory);

    vLog("signal destory() ok!");

    CreateVerText();
    vLog("CreateVerText() ok!");

	return SUCESS;
}
/**
 *初始化日志
 */
static int initlog() {

	char CurPath[FILE_PATH_CHARNUM] = { 0 };
	char LogDirPath[FILE_PATH_CHARNUM] = { 0 };

	GetProgramPath(CurPath,POKA_HOME,DEF_INSTALL_PATH);

	sprintf(LogDirPath, "%s/%s/%s/", CurPath, LOGFOLDER_NAME,APP_NAME);

	if (JudgeSavePathExist(LogDirPath) == ERROR) {
		return ERROR;
	}
	sprintf(G_LOGPATH, "%s", LogDirPath);

	return initPokaLog();
}

static int initpath()
{
	char temPath[FILE_PATH_CHARNUM] = { 0 };

	sprintf(temPath, "%s/%s/", g_param.FileStoreBasePath,g_param.InsertDir);
	if (JudgeSavePathExist(temPath) == ERROR) {
		return ERROR;
	}
	sprintf(temPath, "%s/%s/", g_param.FileStoreBasePath,g_param.handleDir);
	if (JudgeSavePathExist(temPath) == ERROR) {
		return ERROR;
	}
	sprintf(temPath, "%s/%s/", g_param.FileStoreBasePath,g_param.SendFileDIr);
	if (JudgeSavePathExist(temPath) == ERROR) {
		return ERROR;
	}
	sprintf(temPath, "%s/%s/", g_param.FileStoreBasePath,g_param.SwitchDir);
	if (JudgeSavePathExist(temPath) == ERROR) {
		return ERROR;
	}
	sprintf(temPath, "%s/%s/", g_param.FileStoreBasePath,g_param.UpdateDIr);
	if (JudgeSavePathExist(temPath) == ERROR) {
		return ERROR;
	}
	sprintf(temPath, "%s/%s/", g_param.FileStoreBasePath,g_param.ErrorDir);
	if (JudgeSavePathExist(temPath) == ERROR) {
		return ERROR;
	}

	sprintf(temPath, "%s/%s/", g_param.ImagePath,g_param.ImageDir);
	if (JudgeSavePathExist(temPath) == ERROR) {
		return ERROR;
	}

	if(g_param.openTransfom !=0){
		sprintf(temPath, "%s/%s/", g_param.FileStoreBasePath,g_param.transformDir);
		if (JudgeSavePathExist(temPath) == ERROR) {
			return ERROR;
		}
		sprintf(temPath, "%s/%s/", g_param.FileStoreBasePath,g_param.transformFinDir);
		if (JudgeSavePathExist(temPath) == ERROR) {
			return ERROR;
		}
	}
	return SUCESS;
}

void printRout(ROUTE *route )
{
	vLog("SendTask start:route->id[%d],route->ipaddr[%s],route->port[%s],route->user[%s]\
			route->password[%s],route->localdir[%s],route->remotedir[%s]",route->id
			,route->ipaddr,route->port,route->user,route->password
			,route->localdir,route->remotedir);

}

static int initcfg()
{
	char rcfg[FILE_PATH_CHARNUM] = { 0 };
	char apppath[FILE_PATH_CHARNUM] = { 0 };

	int iRet = ReadParamFromIniFile(&g_param,POKA_HOME);
	if(iRet == SUCESS)
	{
		sprintf(rcfg,"%s/%s/%s", GetProgramPath(apppath,POKA_HOME,DEF_INSTALL_PATH),CFG_DIR, ROUTE_CONFILE_NAME);
		FILE *f=fopen(rcfg,"rb");fseek(f,0,SEEK_END);long len=ftell(f);fseek(f,0,SEEK_SET);
		char *data=(char*)malloc(len+1);fread(data,1,len,f);fclose(f);
		cJSON *json;
		json=cJSON_Parse(data);
		if (!json)
		{
			vLog("Parse Json Error before: [%s]\n",cJSON_GetErrorPtr());
			free(data);
			return ERROR;
		}
		else
		{
			ROUTE *ro;
			int iSize = cJSON_GetArraySize(json);
			int iCnt;
			for(iCnt = 0; iCnt < iSize; iCnt++)
			{

				cJSON * pSub = cJSON_GetArrayItem(json, iCnt);
				if(NULL == pSub)
				{
					 continue;
				}
				ro = (ROUTE *)malloc(sizeof(ROUTE));
				if(ro == NULL)
				{
					vLog("Malloc route error");
					return ERROR;
				}
				memset(ro,0x00,sizeof(ROUTE));
				ro->id = iCnt;
				cJSON * item = cJSON_GetObjectItem(pSub, "targetbank");
				if(item)
				{
					memcpy(ro->targetbank,item->valuestring,strlen(item->valuestring));
					strtrim(ro->targetbank);
				}
				item = cJSON_GetObjectItem(pSub, "forwardbank");
				if(item)
				{
					memcpy(ro->forwardbank,item->valuestring,strlen(item->valuestring));
					strtrim(ro->forwardbank);
				}
				item = cJSON_GetObjectItem(pSub, "ipaddr");
				if(item)
				{
					memcpy(ro->ipaddr,item->valuestring,strlen(item->valuestring));
					strtrim(ro->ipaddr);
				}
				item = cJSON_GetObjectItem(pSub, "port");
				if(item)
				{
					memcpy(ro->port,item->valuestring,strlen(item->valuestring));
					strtrim(ro->port);
				}
				item = cJSON_GetObjectItem(pSub, "user");
				if(item)
				{
					memcpy(ro->user,item->valuestring,strlen(item->valuestring));
					strtrim(ro->user);
				}
				item = cJSON_GetObjectItem(pSub, "password");
				if(item)
				{
					memcpy(ro->password,item->valuestring,strlen(item->valuestring));
					strtrim(ro->password);
				}
				item = cJSON_GetObjectItem(pSub, "localdir");
				if(item)
				{
					memcpy(ro->localdir,item->valuestring,strlen(item->valuestring));
					strtrim(ro->localdir);
				}
				item = cJSON_GetObjectItem(pSub, "remotedir");
				if(item)
				{
					memcpy(ro->remotedir,item->valuestring,strlen(item->valuestring));
					strtrim(ro->remotedir);
				}
				item = cJSON_GetObjectItem(pSub, "type");
				if(item)
				{
					ro->type=atoi(item->valuestring);
				}
				item = cJSON_GetObjectItem(pSub, "servicecode");
				if(item)
				{
					ro->servicecode=atoi(item->valuestring);
				}
				item = cJSON_GetObjectItem(pSub, "lastdate");
				if(item)
				{
					ro->lastdate=atoi(item->valuestring);
				}
				item = cJSON_GetObjectItem(pSub, "starttime");
				if(item)
				{
					ro->starttime=atoi(item->valuestring);
				}
				item = cJSON_GetObjectItem(pSub, "inter_val");
				if(item)
				{
					ro->interval=atoi(item->valuestring);
				}
				item = cJSON_GetObjectItem(pSub, "model");
				if(item)
				{
					ro->model=atoi(item->valuestring);
				}
				ro->status = S_STOP;
				ADD_ROUTE(ro);
			}
			cJSON_Delete(json);
		}
		free(data);
	}
	return SUCESS;
}


static int initThread()
{
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	return SUCESS;
}
static void CreateVerText()
{
	FILE *pVerText = NULL;

	pVerText = fopen(APP_VERSION, "w+");
	if (NULL == pVerText) {
		printf("Create version text fail\n");
		vLog("Create version text fail");
	} else {
		printf("Create version text ok or text exist\n");
		vLog("Create version text ok or text exist\n");
		fclose(pVerText);
		pVerText = NULL;
	}
}

static void *sendThread()
{

	vLog("sendThread   start");

	int iRet,i;
	pthread_t service;

	while (1) {
		ROUTE *ro;
		unsigned int time = GetTimeInt();
		unsigned int date = GetDateInt();

		for(i=0;i<g_arr.len;i++)
		{
			ro = GET_ROUTE(i);
			vLog("cur time[%d],cur date[%d],route[%d] start after[%d]",time,date,ro->id,ro->starttime);
			if(ro->status == S_STOP)
			{
				if(ro->starttime > time)
				{
					continue;
				}
				if(ro->interval == 0 && ro->lastdate == date )
				{
					continue;
				}
				Start_ftp_service(ro);
				pthread_create(&service, &attr, SendTaskThread, (void *) (ro));
			}

		}
		sleep(60);
	}

	return (void *) 0;
}

void *SendTaskThread(void *sp)
{
	ROUTE *route = (ROUTE *) sp;

	char tBank[20 + 1] = { 0 };
	char fBank[20 + 1] = { 0 };

	printRout(route);

	Start_ftp_service(route);
	if (strlen(route->ipaddr) <= 0)
	{
		vLog("route[%d]  ipaddr [%s] is null ", route->id, route->ipaddr);
		Stop_ftp_service(route);
		return (void *) ERROR;
	}

	if (strlen(route->localdir) <= 0 || access(route->localdir, 0) != 0) {
		vLog("route[%d] can't access localdir [%s] ", route->id,
				route->localdir);
		Stop_ftp_service(route);
		return (void *) ERROR;
	}

	if (strlen(route->forwardbank) > 0) {
		memcpy(fBank, route->forwardbank, strlen(route->forwardbank));
	} else {
		if (strlen(route->targetbank) > 0) {
			memcpy(fBank, route->targetbank, strlen(route->targetbank));
		} else {
			vLog("route[id = %d] get forwardbank error ", route->id);
			Stop_ftp_service(route);
			return (void *) ERROR;
		}
	}
	if (strlen(route->targetbank) > 0) {
		memcpy(tBank, route->targetbank, strlen(route->targetbank));
	} else {
		memcpy(tBank, route->forwardbank, strlen(route->forwardbank));
	}

	JudgeSavePathExist(route->localdir);

	vLog("Get ROUTERULE[%d]",route->id);
	ROUTERULE rule[MAX_RULE];
	int ruleSize;
	memset(&rule, 0x00, sizeof(ROUTERULE) * MAX_RULE);

	ruleSize=0;

	int iRet;
	switch (route->type) {
	case FTP_UPLOAD:
	case SFTP_UPLOAD:
		iRet = UploadFile(route,&rule,ruleSize);
		break;
	case FTP_DOWNLOAD:
	case SFTP_DOWNLOAD:
		iRet = DownFile(route,&rule,ruleSize);
		break;
	default:
		vLog("route[id = %d] get type [%d] error ", route->id, route->type);
		Stop_ftp_service(route);
		return (void *) 0;
	}
	vLog("SUCESS route[id = %d] get interval [%d] iRet[%d] ", route->id, route->interval,iRet);
	if(iRet == SUCESS)
	{

		if(route->interval!=0)
		{
			route->starttime = GetTimeInterval(route->interval);
		}else{
			route->lastdate = GetDateInt();
		}
		vLog("SUCESS route[id = %d] get interval [%d]  ", route->id, route->interval);
	}else{
		vLog("SendTask error:id[%d]",route->id);
	}
	Stop_ftp_service(route);
	return (void *) 0;
}

static int Start_ftp_service(ROUTE *route)
{
	route->status = S_START;
	return SUCESS;
}
static int Stop_ftp_service(ROUTE *route)
{
	route->status = S_STOP;
	return SUCESS;
}
