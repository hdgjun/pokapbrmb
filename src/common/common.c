/*
 * common.c
 *
 *  Created on: 2016年6月20日
 *      Author: Administrator
 */
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <signal.h>
#include "switch.h"
#include "common.h"


static int CheckServer(int iFlag,const char *appName);
static int IsServer(char *pszDir,const char *appName);


char *GetProgramPath(char *path,const char *enName,const char *def)
{
	char *env = getenv(enName);
	if(env){
		memcpy(path,env,strlen(env));
	}else{
		memcpy(path, def,strlen(def));
	}
	return path;
}

/*pthread_kill的返回值：成功（0） 线程不存在（ESRCH） 信号不合法（EINVAL）*/
int test_pthread(pthread_t tid)
{

	if (tid == NULL) {
		printf("thread not exist");
		return ESRCH;
	}
	int pthread_kill_err;
	pthread_kill_err = pthread_kill(tid, 0);

	if (pthread_kill_err == ESRCH) {
		printf("thread not exist or exit thread ID:0x%x\n", (unsigned int) tid);
		return ESRCH;
	} else if (pthread_kill_err == EINVAL) {
		//printf("发送信号非法。\n");
		printf("send signal illegal\n");
		return EINVAL;
	} else {
		printf("thread still live thread ID:0x%x, 0x%x\n", (unsigned int) tid,pthread_kill_err);
		return 0;
	}
}

void psleep(int timeOut)
{
	waitfortime wt;
	pthread_mutex_init(&(wt.mutex), NULL);

	pthread_condattr_init(&(wt.conattr));

	pthread_condattr_setclock(&(wt.conattr), CLOCK_MONOTONIC);
	pthread_cond_init(&(wt.cond), &(wt.conattr));

	struct timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);
	tv.tv_sec += timeOut;
	pthread_cond_timedwait(&(wt.cond), &(wt.mutex), &tv);
}


int JudgeProgramExist(int iFlag,const char *appName) {
	if (1 != CheckServer(iFlag,appName)) {
		printf("No certain %s is running\n",appName);
	} else {
		printf("The program %s is running!\n",appName);
		return ERROR;
	}
	return SUCESS;
}

int CheckServer(int iFlag,const char *appName)
{
	int iRet = 0;
	DIR *dir;
	struct dirent *dt;
	unsigned int uicount = 0;

	if((dir = opendir("/proc")) == NULL)
	{
		return(-1);
	}

	while((dt=readdir(dir)) != NULL)
	{
		struct stat statbuf;
		char szFileName[FILE_PATH_CHARNUM+1] = {0};

		sprintf(szFileName,"%s/%s","/proc", dt->d_name);
		if(!isdigit((int)dt->d_name[0]))
		{
			continue;
		}
		if (stat(szFileName, &statbuf) != 0)
		{
			continue;
		}

		if (S_ISDIR(statbuf.st_mode))
		{
			if(1 == IsServer(szFileName,appName))
			{
				switch(iFlag)
				{
					case 0:
						return 1;
					case 1:
						uicount++;
						if(2 <= uicount)
						{
							closedir(dir);
							dir = NULL;
							return 1;
						}
						break;
					default:
						printf("CheckServer iFlag error:%d\n",iFlag);
						break;
				}
			}
		}
	}
	closedir(dir);
	dir = NULL;

	return iRet;
}


static int IsServer(char *pszDir,const char *appName)
{
	char szPath[FILE_PATH_CHARNUM+1] = {0};
	char szBuf[1024] = {0};
	int i = 0;
	sprintf(szPath, "%s/%s", pszDir, "status");
	FILE *pFile = fopen(szPath, ONLYREAD_ACCESS_STRING);
	if(pFile)
	{
		for(i = 0; i < 1024; i++)
		{
			szBuf[i] = fgetc(pFile);
			if(szBuf[i] == '\r' || szBuf[i] == '\n')
				break;
		}
		fclose(pFile);
		pFile = NULL;
	}

	memset(&szPath, 0x00, FILE_PATH_CHARNUM+1);
	sscanf(szBuf,"%*s%s", szPath);
	if(0 == strcmp(szPath, appName))
	{
		return 1;
	}

	return 0;
}
