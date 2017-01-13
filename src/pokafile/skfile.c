/*
 * skfile.c
 *
 *  Created on: 2017Äê1ÔÂ13ÈÕ
 *      Author: Administrator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "db/db.h"
#include "pokafile.h"
#include "switch.h"
#include "db/dbskjl.h"

#define SK_RECORD_SIZE 77

static int	ReadSKFileName(const char *szFileName, FILENAME *pfilename);
static int MoveSKFile(FILENAME *fn,int result);
static char *GetSKSucInsLocalDir(char *dir,const  FILENAME *fn);
static int ReadSkContents(FILENAME *fn);
int SKFile(DataType *df)
{
	FILENAME fn;
	if(df->fileType != SK_FILE_TYPE)
	{
		return WARING;
	}
	memset(&fn,0x00,sizeof(FILENAME));
	ReadSKFileName(df->fileName,&fn);
	fn.df = df;
	int iRet = ReadSkContents(&fn);

	if(MoveSKFile(&fn,iRet) != SUCESS)
	{
		MoveSKFile(&fn,iRet);//try again
	}
	return iRet;
}
static int ReadSkContents(FILENAME *fn)
{
	char strSkFilePath[FILE_PATH_CHARNUM] = {0};
	char buf[SK_RECORD_SIZE+1] = {0};
    int rest;
	sprintf(strSkFilePath,"%s/%s.%s",fn->df->filePath,fn->df->fileName,START_FILE_STRING);

	FILE *pfile;
	pfile = fopen(strSkFilePath,ONLYREAD_ACCESS_STRING);
	if(pfile == NULL)
	{
		printf("Open BK file fail:%s\n",strerror(errno));
		vLog("Open BK file fail:%s",strerror(errno));
		return ERROR;
	}

	cir_queue_t que;
	init_cir_queue(&que);
	DBSKJL *data;
	int size;
	int lo;
	while(1)
	{
		memset(buf,0x00,SK_RECORD_SIZE+1);
		size = fread(buf,SK_RECORD_SIZE,1,pfile);
		vLog(" sk file[%s] format error[%d:%s]",fn->df->fileName,strlen(buf),buf);
		if(size == 0)
		{
			if(strlen(buf) != (SK_RECORD_SIZE-1))
			{
				fclose(pfile);
				if(strlen(buf) == 0)
				{
				  break;
				}
				destroy_cir_queue(&que);
				vLog(" sk file[%s] format error[%d:%s]",fn->df->fileName,strlen(buf),buf);
				return ERROR;
			}
		}
		data = (DBSKJL *)malloc(sizeof(DBSKJL));
		if(data == NULL)
		{
			 fclose(pfile);
			 destroy_cir_queue(&que);
			 vLog("malloc SK file[%s] error[%s]",fn->df->fileName,strerror(errno));
			 return WARING;
		}
		memset(data,0x00,sizeof(DBSKJL));
		memcpy(data->bankno,fn->BankNo,strlen(fn->BankNo));
		memcpy(data->notno,fn->AgencyNo,strlen(fn->AgencyNo));

		lo=0;
		memcpy(data->businessdate,buf+lo,BUSINESS_DATE_LEN);
		lo+=BUSINESS_DATE_LEN;
		memcpy(data->userid,buf+lo,USER_ID_LEN);
		lo+=USER_ID_LEN;
		memcpy(data->accountno,buf+lo,ACCOUNT_NO_LEN);
		lo+=ACCOUNT_NO_LEN;
		memcpy(data->percode,buf+lo,PERCODE_LEN);
		strtrim(data->accountno);
		strtrim(data->businessdate);
		strtrim(data->userid);
		strtrim(data->percode);
		push_cir_queue(&que,data);

	}

	while(1)
	{
		data = pop_notwait_cir_queue(&que);
		if(data == NULL)
		{
			break;
		}
		rest = DbSKJL(DBS_INSERT,data);
		if (rest == WARING)
		{
			if(data)free(data);
			destroy_cir_queue(&que);
			DbsRollback();
			return WARING;
		}
		if(data)free(data);
	}

	destroy_cir_queue(&que);
	rest = DbsCommit();
	return rest;
}
int	ReadSKFileName(const char *szFileName, FILENAME *pfilename)
{
	char strFileName[FILE_PATH_CHARNUM] = {0};
	char *pLeftStr = NULL;

	strcpy(strFileName,szFileName);

	memset(pfilename,0x00,sizeof(FILENAME));

	char *pReturn1 ;
	pReturn1 = strtok_r(strFileName, SEPARATOR_STRING, &pLeftStr);
	if(pReturn1 != NULL)
	{
		strcpy(pfilename->BankNo,pReturn1);
	}
	pReturn1 = strtok_r(NULL, SEPARATOR_STRING, &pLeftStr);
	if(pReturn1!=NULL)
	{
	  strcpy(pfilename->AgencyNo,pReturn1);
	}

	pReturn1 = strtok_r(NULL, SEPARATOR_STRING, &pLeftStr);
	if(pReturn1!=NULL)
	{
	  strcpy(pfilename->PackageId,pReturn1);
	  memset(pfilename->Date,0,DATE_LEN+1);
	  sprintf(pfilename->Date,"20");
	  memcpy(pfilename->Date+2,pReturn1,DATE_LEN-2);
	}
	return 0;
}

static int MoveSKFile(FILENAME *fn,int result)
{
	char dir[FILE_PATH_CHARNUM] = {0};
	FILEMOVEINFO fe;
	memset(&fe,0x00,sizeof(FILEMOVEINFO));
	switch (result) {
		case ERROR:
			GetErrorLocalDir(dir,fn);
			sprintf(fe.LocalDesBaseName,"%s",fn->df->fileName);
			memcpy(fe.LocalDesFilePath,dir,strlen(dir));
			break;
		case SUCESS:
			GetSKSucInsLocalDir(dir,fn);
			sprintf(fe.LocalDesBaseName,"%s",fn->df->fileName);
			memcpy(fe.LocalDesFilePath,dir,strlen(dir));
		break;
		case WARING:return result;
	}
	sprintf(fe.FileBaseName,"%s.%s",fn->df->fileName,START_FILE_STRING);
	memcpy(fe.FilePth,fn->df->filePath,strlen(fn->df->filePath));
	fe.isRemote = 0;
	return MoveFiles(&fe);
}

static char *GetSKSucInsLocalDir(char *dir,const  FILENAME *fn)
{
	char bankno[BANKNO_LEN+1] = {0};
	char agencyno[AGENCYNO_LEN+1] ={0};
	char temDir[FILE_PATH_CHARNUM] = {0};

	char newDate[9] = { 0 };
	GetGiveDate(newDate);
	memcpy(bankno,fn->BankNo,strlen(fn->BankNo));
	memcpy(agencyno,fn->AgencyNo,strlen(fn->AgencyNo));

	sprintf(temDir,"%s/%s/%s/%s/%s/%s/",g_param.FileStoreBasePath,g_param.handleDir
			,bankno,agencyno,newDate,fn->Date);
	memcpy(dir,temDir,strlen(temDir));
	JudgeSavePathExist(temDir);
	return dir;
}

