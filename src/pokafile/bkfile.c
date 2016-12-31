#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "pokafile.h"
#include "db/db.h"
#include "db/dbpackageinfo.h"
#include "switch.h"

static int ReadBkContents(FILENAME *fn);
int BKFile(DataType *df)
{
	FILENAME fn ;

	if(df->fileType != BK_FILE_TYPE)
	{
		return WARING;
	}

	memset(&fn,0x00,sizeof(FILENAME));
	ReadFSNOrBKFileName(df->fileName,&fn);
	fn.df = df;

	int iRet = ReadBkContents(&fn);
	if(MoveFSNFile(&fn,iRet) != SUCESS){
			MoveFSNFile(&fn,iRet);//try again
	}
	return iRet;
}

static int ReadBkContents(FILENAME *fn)
{
	char strBkFilePath[FILE_PATH_CHARNUM] = {0};

	sprintf(strBkFilePath,"%s/%s.%s",fn->df->filePath,fn->df->fileName,START_FILE_STRING);

	FILE *pfile;
	pfile = fopen(strBkFilePath,ONLYREAD_ACCESS_STRING);
	if(pfile == NULL)
	{
		printf("Open BK file fail:%s\n",strerror(errno));
		vLog("Open BK file fail:%s",strerror(errno));
		return ERROR;
	}

	char buf[1024];
	BUNDLEBOBINFO bundlebobinfo;
	memset(&bundlebobinfo,0,sizeof(BUNDLEBOBINFO));

	if(NULL == fgets(buf, sizeof(buf),pfile))
	{
		fclose(pfile);
		pfile = NULL;
		return ERROR;
	}
	if(1 >= strlen(buf))//¿ÕÐÐ
	{
		printf("Packageid is null!\n");
		vLog("Packageid is null!");
	}
	memcpy(bundlebobinfo.BundleId,buf,24);
	bundlebobinfo.BundleId[24] = 0;
	memset(buf,0x00,sizeof(buf));
#ifdef DEBUG
	vLog("Packageid :%s ",bundlebobinfo.BundleId);
#endif
	if(NULL == fgets(buf, sizeof(buf),pfile))
	{
		fclose(pfile);
		pfile = NULL;
		return ERROR;
	}
	memcpy(bundlebobinfo.BundleDate,buf,19);
	bundlebobinfo.BundleDate[19] = 0;
#ifdef DEBUG
	vLog("BundleDate :%s ",bundlebobinfo.BundleDate);
#endif
	memset(buf,0x00,sizeof(buf));
	if(NULL == fgets(buf, sizeof(buf),pfile))
	{
		fclose(pfile);
		pfile = NULL;
		return ERROR;
	}
	memcpy(bundlebobinfo.OperatorId,buf,24);
	bundlebobinfo.OperatorId[24]=0;
#ifdef DEBUG
	vLog("OperatorId :%s ",bundlebobinfo.OperatorId);
#endif
	memset(buf,0x00,sizeof(buf));
	if(NULL == fgets(buf, sizeof(buf),pfile))
	{
		fclose(pfile);
		pfile = NULL;
		return ERROR;
	}

	memcpy(bundlebobinfo.Checker,buf,24);
	bundlebobinfo.Checker[24] = 0;
#ifdef DEBUG
	vLog("Checker :%s ",bundlebobinfo.Checker);
#endif
	int count = 0;
	int k;
	int iOneKunBaNum = atoi(fn->CounterNo);
	int iRet;
#ifdef DEBUG
	vLog("iOneKunBaNum :%d ",iOneKunBaNum);
#endif
	for(k = 0 ;k<iOneKunBaNum;k++)
	{
		memset(buf,0x00,sizeof(buf));
		if(fgets(buf, sizeof(buf),pfile))
		{
			if(1 >= strlen(buf))//¿ÕÐÐÌø¹ý
			{
				printf("Baid is null!\n");
				vLog("Baid is null!");
				continue;
			}

			DBPACKAGEINFO info;
			memset(&info,0x00,sizeof(DBPACKAGEINFO));

			memcpy(info.bundleId,buf,24);
			info.bundleId[24]=0;
#ifdef DEBUG
	vLog("bundleId :%s ",info.bundleId);
#endif
			memcpy(info.packageId,bundlebobinfo.BundleId,24);
			memcpy(info.checker,bundlebobinfo.Checker,strlen(bundlebobinfo.Checker));
			memcpy(info.fileName,fn->df->fileName,strlen(fn->df->fileName));
			memcpy(info.operatorid,bundlebobinfo.OperatorId,strlen(bundlebobinfo.OperatorId));
			memcpy(info.opertime,bundlebobinfo.BundleDate,strlen(bundlebobinfo.BundleDate));
			if(DbPackageinfo(DBS_INSERT,&info)==WARING)
			{
				if(NULL != pfile)
				{
					fclose(pfile);
					pfile = NULL;
				}
				return WARING;
			}

		}
	}

	iRet = DbsCommit();

	if(NULL != pfile)
	{
		fclose(pfile);
		pfile = NULL;
	}
	return iRet;
}
