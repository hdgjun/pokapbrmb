/*
 * common.c
 *
 *  Created on: 2016年6月20日
 *      Author: Administrator
 */
#include <stdio.h>
#include <stdlib.h>
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
#include "pokafile.h"

static int CheckServer(int iFlag,const char *appName);
static int IsServer(char *pszDir,const char *appName);


int my_system(const char * cmd)
{
	FILE * fp;
	int res;
	char buf[2048];

	if (cmd == NULL)
	{
		printf("my_system cmd is NULL!\n");
		return ERROR;
	}
	if ((fp = popen(cmd, "r") ) == NULL)
	{
		perror("popen");
		printf("popen error: %s/n", strerror(errno));
		return ERROR;
	}
	else
	{
		while(fgets(buf, sizeof(buf), fp))
		{
			printf("%s", buf);
		}
		if ( (res = pclose(fp)) == -1)
		{
			printf("close popen file pointer fp error!\n"); return res;
		}
		else if (res == 0)
		{
			return res;
		}
		else
		{
			printf("popen res is :%d\n", res); return res;
		}
	}
}

int CheckFileType(char *fileType)
{
	if(fileType == NULL)return ERROR;
	if (strstr(fileType, FSN_FILE_STRING) != 0) {
		return FSN_FILE_TYPE;
	} else if (strstr(fileType, ZIP_FILE_STRING) != 0) {
		return ZIP_FILE_TYPE;
	} else if (strstr(fileType, BF_FILE_STRING) != 0) {
		return BF_FILE_TYPE;
	} else if (strstr(fileType, BK_FILE_STRING) != 0) {
		return BK_FILE_TYPE;
	} else if (strstr(fileType, CT_FILE_STRING) != 0) {
		return CT_FILE_TYPE;
	} else if (strstr(fileType, START_FILE_STRING) != 0) {
		return START_FILE_TYPE;
	}else if (strstr(fileType, SK_FILE_STRING) != 0) {
		return SK_FILE_TYPE;
	}
#ifndef  PEOPLEBANK
	else if (strstr(fileType, DK_FILE_STRING) != 0) {
		return DK_FILE_TYPE;
	}
#endif
	return ERROR;
}

int unZip(DataType *df)
{
	char cmd[MAX_STRING_SIZE] = {0};
	char udir[CUR_STRING_SIZE] = {0};
	char srcfile[CUR_STRING_SIZE] = {0};
	int iRet = SUCESS;

	sprintf(udir,"%s/%s_%s/", df->filePath,df->fileName,START_FILE_STRING);
	sprintf(srcfile,"%s/%s", df->filePath,df->fileName);
	sprintf(cmd, "unzip -o %s -d %s", srcfile,udir);

	my_system(cmd);
	if(access(udir,0)!=0)
	{
		return ERROR;
	}
	memset(cmd,0x00,MAX_STRING_SIZE);
	sprintf(cmd, "mv %s/* %s", udir,df->filePath);
	my_system(cmd);
	if(CheckDirEmpty(udir) == ERROR)
	{
		iRet = ERROR;
	}
	memset(cmd,0x00,MAX_STRING_SIZE);
	sprintf(cmd, "rm -rf %s/", udir);
	my_system(cmd);

    if(iRet == SUCESS)
    {
		memset(cmd,0x00,MAX_STRING_SIZE);
		sprintf(cmd, "rm -rf %s", srcfile);
		my_system(cmd);
    }

	return iRet;
}

int CheckDirEmpty(char *dir)
{
	DIR* p;
	struct dirent* dirlist;
	struct stat filestat;
	char indir[FILE_PATH_CHARNUM] = { 0 };
	p = opendir(dir);
	if (p == NULL) {
		return SUCESS;
	}
	int iRet = SUCESS;
	while ((dirlist = readdir(p)) != NULL) {
		sprintf(indir, "%s/%s", dir, dirlist->d_name);
		stat(indir, &filestat);
		if (S_ISREG(filestat.st_mode)) {
			if (p != NULL) {
				closedir(p);
				p = NULL;
			}
			return ERROR;
		}
	}
	if (p != NULL) {
		closedir(p);
		p = NULL;
	}
	return iRet;
}
int CheckZipFIle(char *path,char *zfile,char *sfile,int size)
{
	char cmd[MAX_STRING_SIZE] = {0};
	char udir[CUR_STRING_SIZE] = {0};
	char srcfile[CUR_STRING_SIZE] = {0};
	char srczipfile[CUR_STRING_SIZE] = {0};
	struct stat filestat;
	int iRet = SUCESS;

	int time = GetTimeInt();

	sprintf(udir,"%s/%u_%d/", path,(unsigned int) pthread_self(),time);
	sprintf(srczipfile,"%s/%s", path,zfile);
	sprintf(cmd, "unzip -o %s -d %s", srczipfile,udir);
	my_system(cmd);

	if(access(udir,0)!=0)
	{
		return ERROR;
	}

	sprintf(srcfile,"%s/%s",udir,sfile);

	stat(srcfile, &filestat);
	int cusize = filestat.st_size;

	memset(cmd,0x00,MAX_STRING_SIZE);
	sprintf(cmd, "rm -rf %s/", udir);
	my_system(cmd);

	if(cusize == size)
	{
		return SUCESS;
	}else{
		return ERROR;
	}
}
int CompressFile(char *srcFile,char *zipFile)
{
	char cmd[MAX_STRING_SIZE] = {0};
	sprintf(cmd,"%s -rj %s %s",ZIPCMD_STRING,zipFile,srcFile);
	printf("CompressFile CompressStr:%s\n",cmd);
	return my_system(cmd);
}


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
		printf("thread not exist\n");
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
