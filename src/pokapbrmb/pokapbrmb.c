#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "pokapbrmb.h"
#include "allcommon.h"
#include "pokalog.h"
#include "pokafile.h"
#include "db/db.h"
#include "switch.h"
#include "monrule.h"

extern Param g_param;

static int init();
static int initlog();
static int initcfg();
static int initpath();
static int initDataBase();
static int initThread();
static void CreateVerText();


static pthread_t searchfile;
static pthread_t cutDate;
static pthread_t taskfile;
static pthread_t sendfile;
static pthread_t switchfile;
static pthread_t clearFile;
static pthread_t fsnfiletobase;
static pthread_t fileOpr[MAX_THREAD_NUM];

extern pthread_attr_t attr;


extern cir_queue_t gQue;

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

	vLog("********************************START**************************");
    i = 0;
	while(1)
	{
		while(1)
		{
			if(i == 0)
			{
				/*宝嘉fsn转标准fsn*/
				if(g_param.openTransfom != 0)
				{
					if(ESRCH == test_pthread(fsnfiletobase))
					{
						pthread_create(&fsnfiletobase, &attr,
								ListTransFormDirThread, (void *)&fsnfiletobase);
					}
				}

				/*根据路由表转发文件线程*/
				if (ESRCH == test_pthread(sendfile))
				{
					pthread_create(&sendfile, &attr, SendFileThread,NULL);
				}

				/*清理旧文件*/
				if (ESRCH == test_pthread(clearFile))
				{
					pthread_create(&clearFile, &attr, CleanFileThread,NULL);
				}

				if(g_param.ThreadSize > 0)
				{
					updateMonrule();
					vLog("updateMonrule() ok!");

					/*将insertdir:fsn_insert目录文件放入入库队列，等待入库处理*/
					if (ESRCH == test_pthread(taskfile))
					{
						pthread_create(&taskfile, &attr,
								ListDirThread, (void *)&taskfile);
					}
					/*文件分发线程,扫描fsn_put,(商行)发送一份文件到fsn_insert,同时发送一份文件到fsn_sc_ins/人行银行号/ */
					if(ESRCH == test_pthread(switchfile))
					{
						pthread_create(&switchfile, &attr,
								SwitchFileThread, (void *)&switchfile);
					}
#ifndef PEOPLEBANK
					if(g_param.OnlyPaytoPb == DEF_ONLY_PAY_PB)
					{//如果等于1，仅发送交款文件
						pthread_create(&searchfile, &attr,
								SearchFileThread, (void *)&searchfile);
					}
#else
					pthread_create(&searchfile, &attr,
							SearchPayThread, (void *)&searchfile);
#endif
				}
			}
			if(g_param.ThreadSize > 0)
			{
				/*从队列获取任务，处理文件*/
				if (ESRCH == test_pthread(fileOpr[i]))
				{
					pthread_create(&fileOpr[i], &attr, HandleFileThread, (void *)i);
				}
			}

			i++;
			if(i >= g_param.ThreadSize)break;
	   }
		sleep(g_param.SleepTime*60);
	}

	return 0;

}

static void Destory()
{
	printf("Signal deal!\n");
	DestroyDb();
	pthread_attr_destroy(&attr);
	destroy_cir_queue(&gQue);
	printf("Signal was stop by ctrl+c!\n");
	exit(0);
}
static int init() {
	printf("STARTING POKAPBRMB!\n");

	if (initlog() != SUCESS) {
		printf("init log error!\n");
		return ERROR;
	}

	if(initcfg() != SUCESS){
		vLog("init cfg error\n");
		return ERROR;
	}
#ifndef PEOPLEBANK
	if(strlen(g_param.PBno)<=0){
		vLog("People bank no not set!\n");
		return ERROR;
	}
#endif
	vLog("init cfg ok!");

    if(initpath() != SUCESS){
    	vLog("init path error");
    	return ERROR;
    }
    vLog("init path ok!");

    if(initDataBase() != SUCESS){
		vLog("init DB error,please check db configure");
		return ERROR;
    }

    vLog("init DB ok!");
    if(initThread() != SUCESS){
    	vLog("init Thread error");
    	return ERROR;
    }
    vLog("init Thread ok!");

    init_cir_queue(&gQue);

    vLog("task queue ok!");

    signal(SIGINT,Destory);

    vLog("signal destory() ok!");

    CreateVerText();
    vLog("CreateVerText() ok!");

    CoverFiles();
    vLog("CoverFiles() ok!");

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
static int initcfg()
{
	return ReadParamFromIniFile(&g_param,POKA_HOME);
}

static int initDataBase()
{
	char CurPath[FILE_PATH_CHARNUM] = { 0 };
    GetProgramPath(CurPath,POKA_HOME,DEF_INSTALL_PATH);
    char sbin[FILE_PATH_CHARNUM] = { 0 };
    sprintf(sbin,"%s/%s/",CurPath,SHELL_DIR);
    InitDb(sbin);

	return ThreadConnectDB();
}

static int initThread()
{
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	/*日切线程*/
	pthread_create(&cutDate, &attr,date_cut_thread,NULL);

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
