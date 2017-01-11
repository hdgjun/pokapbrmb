/*
 * pokafsntobase.c
 *
 *  Created on: 2017年1月11日
 *      Author: Administrator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pokafile.h"
#include "switch.h"

static int checkFileSize(unsigned long fsize);
static int transFsnFile(DataType *df);
static int moveTransFSNFile(DataType *df,int result);

void *ListTransFormDirThread(void *pt)
{
	DIR* p;
	struct dirent* dirlist;
	struct stat filestat;
	char szFolderPath[FILE_PATH_CHARNUM] = { 0 };
	char indir[FILE_PATH_CHARNUM] = { 0 };



	pthread_t *th = (pthread_t *) pt;

	sprintf(szFolderPath, "%s/%s/", g_param.FileStoreBasePath,
			g_param.transformDir);

#ifdef   DEBUG
	vLog("ListTransFormDirThread:%s", szFolderPath);
#endif

	while (1) {

#ifdef   DEBUG
		vLog("ListTransFormDirThread  opendir");
#endif
		p = opendir(szFolderPath);
		if (p == NULL) {
			printf("ListTransFormDirThread Open dir %s fail:%s\n", szFolderPath,
					strerror(errno));
			vLog("ListTransFormDirThread Open dir %s fail:%s", szFolderPath,
					strerror(errno));
			return (void *) ERROR;
		}
		char *fileType;
		DataType df;
		int fs = 0;
		int tp;
		while ((dirlist = readdir(p)) != NULL)
		{
			sprintf(indir, "%s/%s", szFolderPath, dirlist->d_name);
			stat(indir, &filestat);

			if (S_ISREG(filestat.st_mode)) {
				fileType = strrchr(dirlist->d_name, '.');
				tp = CheckFileType(fileType);

				memset(&df, 0x00, sizeof(DataType));
				memcpy(df.filePath, szFolderPath, strlen(szFolderPath));
				memcpy(df.fileName, dirlist->d_name, strlen(dirlist->d_name));
				df.filesize = filestat.st_size;
				df.fileType = tp;

				if(tp == ZIP_FILE_TYPE)
				{
					if(StartFile(&df)==SUCESS)
					{
						ZIPFile(&df);
					}
					continue;
				}else if(tp!=FSN_FILE_TYPE)
				{
					continue;
				}
				int iRet;
				switch(checkFileSize(df.filesize))
				{
					case 0:
						//标准fsn,不须转化
						moveTransFSNFile(&df,SUCESS);
						break;
					case 1:
						transFsnFile(&df);
						break;
					default:
						moveTransFSNFile(&df,ERROR);
				}
			}

		}

		if (p != NULL) {
			closedir(p);
			p = NULL;
		}
		sleep(60);
	}

	if (th != NULL) {
		memset(th, 0x00, sizeof(pthread_t));
	}
	return (void *) SUCESS;
}
int transFsnFile(DataType *df)
{
	FILE *pf,*bf;

	char pfile[FILE_PATH_CHARNUM] = {0};
	char bfile[FILE_PATH_CHARNUM] = {0};

	char head[FILE_HEAD_SIZE] = {0};
	char precord[ONE_RECORD_SIZE] = {0};
	//char brecord[STANDARD_ONE_RECORD_SIZE] = {0};

	sprintf(pfile,"%s/%s/%s",g_param.FileStoreBasePath,g_param.transformDir,df->fileName);
	sprintf(bfile,"%s/%s/%s.tmp",g_param.FileStoreBasePath,g_param.transformFinDir,df->fileName);

	pf = fopen(pfile,"r");
	if(pf == NULL)
	{
		vLog("can't open file[%s] ",pfile);
		return ERROR;
	}

	bf = fopen(bfile,"w");
	if(bf == NULL)
	{
		vLog("can't open file[%s] ",bfile);
		if(pf)fclose(pf);
		return ERROR;
	}

	int iRet;
	iRet = fread(head,FILE_HEAD_SIZE,1,pf);
	if(iRet!=1)
	{
		if(pf)fclose(pf);
		if(bf)fclose(bf);
		DelFileOrDir(1,bfile);
		vLog("read fsn file[%s] head error ",pfile);
		return ERROR;
	}
	iRet = fwrite(head,FILE_HEAD_SIZE,1,bf);
	if(iRet!=1)
	{
		if(pf)fclose(pf);
		if(bf)fclose(bf);
		DelFileOrDir(1,bfile);
		vLog("write fsn file[%s] head error ",bfile);
		return ERROR;
	}
	while(1)
	{
		iRet = fread(precord,ONE_RECORD_SIZE,1,pf);
		if(iRet == 0)
		{
			break;
		}
		iRet = fwrite(precord,STANDARD_ONE_RECORD_SIZE,1,bf);
		if(iRet!=1)
		{
			if(pf)fclose(pf);
			if(bf)fclose(bf);
			DelFileOrDir(1,bfile);
			vLog("write fsn file[%s] head error ",bfile);
			return ERROR;
		}
	}

	if(pf)fclose(pf);
	if(bf)fclose(bf);

	FILEMOVEINFO fe;
	memset(&fe,0x00,sizeof(FILEMOVEINFO));

	sprintf(fe.FileBaseName,"%s.tmp",df->fileName);
	sprintf(fe.FilePth,"%s/%s",g_param.FileStoreBasePath,g_param.transformFinDir);
	sprintf(fe.LocalDesBaseName,"%s",df->fileName);
	memcpy(fe.LocalDesFilePath,fe.FilePth,strlen(fe.FilePth));
	fe.isRemote = 0;
	iRet = MoveFiles(&fe);

	if(iRet == SUCESS){
		DelFileOrDir(1,pfile);
		vLog("create base fsn file[%s] sucess",bfile);
	}else{
		DelFileOrDir(1,bfile);
		vLog("create base fsn file[%s] faile",bfile);
	}
	return iRet;
}
int moveTransFSNFile(DataType *df,int result)
{
	char dir[FILE_PATH_CHARNUM] = {0};
	FILEMOVEINFO fe;
	memset(&fe,0x00,sizeof(FILEMOVEINFO));
	switch (result) {
		case ERROR:
			GetErrorLocalDir(dir,NULL);
			sprintf(fe.LocalDesBaseName,"%s",df->fileName);
			memcpy(fe.LocalDesFilePath,dir,strlen(dir));
			break;
		case SUCESS:
			sprintf(dir,"%s/%s/",g_param.FileStoreBasePath,g_param.transformFinDir);
			sprintf(fe.LocalDesBaseName,"%s",df->fileName);
			memcpy(fe.LocalDesFilePath,dir,strlen(dir));
			break;
		case WARING:return result;
	}
	sprintf(fe.FileBaseName,"%s",df->fileName);
	memcpy(fe.FilePth,df->filePath,strlen(df->filePath));
	fe.isRemote = 0;
	return MoveFiles(&fe);
}

static int checkFileSize(unsigned long fsize)
{
	int records = fsize - 32;

	if((records%STANDARD_ONE_RECORD_SIZE)==0)
	{
		return 0;
	}else if((records%ONE_RECORD_SIZE)==0){
		return 1;
	}
	return ERROR;
}

