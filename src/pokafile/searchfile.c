/*
 * searchfile.c
 *
 *  Created on: 2017Äê1ÔÂ17ÈÕ
 *      Author: Administrator
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "db/db.h"
#include "pokafile.h"
#include "pokalog.h"
#include "switch.h"
#include "common.h"
#include "db/dborder.h"
#include "db/dbtbundle.h"
#include "db/dbsearchfsnbk.h"
#include "db/dbkunfiles.h"


typedef struct _data_fun
{
	int (*order)(int,char *);
	int (*bundleCo)(int *, char *);
	int (*bundleGetByOrder)(cir_queue_t *, char *);

}data_fun;

void Doit(data_fun *fs);
int GetFile(TSEARCHFSNBK *file);

void *SearchFileThread(void *pt)
{
	pthread_t *th = (pthread_t *) pt;

	data_fun funs;

	TRY_CONNECT_DB

	while(1)
	{
		memset(&funs,0x00,sizeof(data_fun));
		funs.order=DbTrsOrder;
		funs.bundleCo=DbTRSBundleGetCount;
		funs.bundleGetByOrder=DbTRSBundleByOrder;
		Doit(&funs);

		memset(&funs,0x00,sizeof(data_fun));
		funs.order=DbTOrderInfo;
		funs.bundleCo=DbTBundleGetCount;
		funs.bundleGetByOrder=DbTBundleByOrder;
		Doit(&funs);

		sleep(600);
	}

	if (th != NULL)
	{
		memset(th, 0x00, sizeof(pthread_t));
	}
	return (void *)SUCESS;
}

void Doit(data_fun *fs)
{
	if(fs == NULL)
	{
		vLog("Get fun message error!");
		return;
	}

	int iRet;
	int size;
	char orderId[256];
	cir_queue_t que;

	init_cir_queue(&que);

	fs->order(DBS_CURSOR_OPEN,NULL);
	while(1)
	{
		memset(orderId,0x00,sizeof(orderId));

		iRet = fs->order(DBS_FETCH, orderId);
		if (iRet == NODATA || iRet == ERROR)
		{
			#ifdef DEBUG
			vLog("fs->order  DBS_FETCH NODATA [%d]",iRet);
			#endif
			fs->order(DBS_CLOSE, NULL);

			destroy_cir_queue(&que);
			break;
		}

		iRet = fs->bundleCo(&size,orderId);
		if(iRet == ERROR)
		{
			continue;
		}

		iRet = fs->bundleGetByOrder(&que,orderId);
		if(iRet != SUCESS)
		{
			continue;
		}

#ifdef DEBUG
vLog("order[%s]  size[%d]",orderId,size);
#endif
		char *data;
		TSEARCHFSNBK tmp;
		int res;
		for(;;)
		{
			data = (char *)pop_notwait_cir_queue(&que);
			if(data == NULL)
			{
				break;
			}

#ifdef DEBUG
vLog("order[%s]  bundleCode[%s]",orderId,data);
#endif

			memset(&tmp,0x00,sizeof(TSEARCHFSNBK));
			memcpy(tmp.orderId,orderId,strlen(orderId));
			memcpy(tmp.bundleCode,data,strlen(data));
			sprintf(tmp.type,"FSN");

			if(data)
			free(data);

			iRet = DbCheakFsnBk(DBS_SELECT1,&tmp,&res);
			if(iRet != SUCESS)
			{
#ifdef DEBUG
vLog("FSN file not find[%d]",iRet);
#endif
				continue;
			}
			if(res == 0)
			{
				GetFile(&tmp);
			}

			sprintf(tmp.type,"BK");
			iRet = DbCheakFsnBk(DBS_SELECT1,&tmp,&res);
			if(iRet != SUCESS)
			{
#ifdef DEBUG
vLog("BK file not find[%d]",iRet);
#endif
				continue;
			}
			if(res == 0)
			{
				GetFile(&tmp);
			}

		}
		memset(&tmp,0x00,sizeof(TSEARCHFSNBK));
		memcpy(tmp.orderId,orderId,strlen(orderId));
		iRet = DbCheakFsnBk(DBS_SELECT2,&tmp,&res);
		if(iRet != SUCESS)
		{
			continue;
		}

		#ifdef DEBUG
		vLog("order[%s] size[%d] search files[%d]",orderId,size,res);
		#endif

		if(res == size*2)
		{
			fs->order(DBS_UPDATE,orderId);
			DbSearchFsnBk(DBS_DELETE,&tmp);
			DbsCommit();
		}
	}
}

int GetFile(TSEARCHFSNBK *file)
{
	int iRet;
	char tmpc[512] = {0};
	char path[256] = {0};
	char path1[256] = {0};
	char path2[256] = {0};
	TKUNFILES data;

	memset(&data,0x00,sizeof(TKUNFILES));
	memcpy(data.bundleCode,file->bundleCode,strlen(file->bundleCode));
	memcpy(data.type,file->type,strlen(file->type));

	iRet = DbKunFiles(DBS_SELECT,&data);
	if(iRet!=SUCESS)
	{
#ifdef DEBUG
		vLog("Get file[%s][%s] from KUNFILES fail!",data.bundleCode,data.type);
#endif
		return iRet;
	}
	strtrim(data.name);
	strtrim(data.path);

	sprintf(path,"%s/%s/%s",g_param.FileStoreBasePath,g_param.SendFileDIr,g_param.PBno);
	JudgeSavePathExist(path);

	sprintf(path1,"%s/%s.tmp",path,data.name);
	sprintf(path2,"%s/%s",path,data.name);
	sprintf(tmpc,"cp %s/%s %s",data.path,data.name,path1);
	my_system(tmpc);

	if(access(path1,0) == 0)
	{
		sprintf(tmpc,"mv %s %s",path1,path2);
		my_system(tmpc);
		DbSearchFsnBk(DBS_INSERT,file);
		DbsCommit();
	}else{
		vLog("cp file[%s] fail!",path1);
	}

	return SUCESS;
}
