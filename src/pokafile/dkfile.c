#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "pokafile.h"
#include "switch.h"
#include "db/db.h"
#include "db/dbbaginfo.h"

static int	ReadDKFileName(const char *szFileName, FILENAME *pfilename);
static int ReadDkContents(FILENAME *fn);
static int MoveDKFile(FILENAME *fn,int result);
static char *GetDKSucInsLocalDir(char *dir,const  FILENAME *fn);
int DKFile(DataType *df)
{
	FILENAME fn;
	if(df->fileType != DK_FILE_TYPE)
	{
		return WARING;
	}

	memset(&fn,0x00,sizeof(FILENAME));
	ReadDKFileName(df->fileName,&fn);
	fn.df = df;
	int iRet = ReadDkContents(&fn);
	if(MoveDKFile(&fn,iRet) != SUCESS){
		MoveDKFile(&fn,iRet);//try again
	}
	return iRet;
}

static int ReadDkContents(FILENAME *fn)
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

	long infoNum = 0;
	char buf[137];
	memset(buf,0x00,sizeof(buf));

	if(NULL == fgets(buf, sizeof(buf),pfile))
	{
		printf("Read DK file fail:%s\n",strerror(errno));
		vLog("Read DK file fail:%s",strerror(errno));
		fclose(pfile);
		pfile = NULL;
		return ERROR;
	}

	infoNum = atol(buf);

	if(0 >= infoNum)
	{
		printf("DK file infoNum error:%ld\n",infoNum);
		vLog("DK file infoNum error:%ld",infoNum);
		fclose(pfile);
		pfile = NULL;
		return ERROR;
	}

	vLog("DK file infoNum:%d",infoNum);

	BAGINFO dkinfo;
	int rest;
	for(;;)
	{
		memset(&dkinfo,0,sizeof(BAGINFO));
		memset(buf,0x00,sizeof(buf));
		if(NULL == fgets(buf, sizeof(buf)-1,pfile))
		{
			fclose(pfile);
			pfile = NULL;
			break;
		}
		if(1 >= strlen(buf))//空行
		{
			printf("Bagid is null!\n");
			vLog("Bagid is null!");
		}
		memcpy(dkinfo.BagCode,buf,20);
		dkinfo.BagCode[20] = 0;
		//printf("--->222\n");
		memset(buf,0x00,sizeof(buf));
		if(NULL == fgets(buf, sizeof(buf),pfile))
		{
			fclose(pfile);
			pfile = NULL;
			break;
		}
		memcpy(dkinfo.BankId,buf,4);
		dkinfo.BankId[4] = 0;

		memset(buf,0x00,sizeof(buf));
		if(NULL == fgets(buf, sizeof(buf),pfile))
		{
			fclose(pfile);
			pfile = NULL;
			break;
		}
		memcpy(dkinfo.BagDate,buf,19);
		dkinfo.BagDate[19] = 0;

		memset(buf,0x00,sizeof(buf));
		if(NULL == fgets(buf, sizeof(buf),pfile))
		{
			fclose(pfile);
			pfile = NULL;
			break;
		}
		memcpy(dkinfo.BagBagId,buf,8);
		dkinfo.BagBagId[8] = 0;

		memset(buf,0x00,sizeof(buf));
		if(NULL == fgets(buf, sizeof(buf),pfile))
		{
			fclose(pfile);
			pfile = NULL;
			break;
		}
		memcpy(dkinfo.BagCheckId,buf,8);
		dkinfo.BagCheckId[8] = 0;

		memset(buf,0x00,sizeof(buf));
		if(NULL == fgets(buf, sizeof(buf),pfile))
		{
			fclose(pfile);
			pfile = NULL;
			break;
		}
		if(1 >= strlen(buf))//空行
		{
			printf("KunId is null!\n");
			vLog("KunId is null!");
			//continue;//这里不能continue，否则后面乱序
		}
		memcpy(dkinfo.BundleCode,buf,24);
		dkinfo.BundleCode[24] = 0;

		memset(buf,0x00,sizeof(buf));
		if(NULL == fgets(buf, sizeof(buf),pfile))
		{
			fclose(pfile);
			pfile = NULL;
			break;
		}
		memcpy(dkinfo.BundleDate,buf,19);
		dkinfo.BundleDate[19] = 0;

		memcpy(dkinfo.FileName,fn->df->fileName,strlen(fn->df->fileName));
		rest = DbBaginfo(DBS_INSERT,&dkinfo);
		if (rest == WARING)
		{
			if(NULL != pfile)
			{
				fclose(pfile);
				pfile = NULL;
			}
			return WARING;
		}
	}

	rest = DbsCommit();

	if(NULL != pfile)
	{
		fclose(pfile);
		pfile = NULL;
	}
	return rest;
}
static int MoveDKFile(FILENAME *fn,int result)
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
			GetDKSucInsLocalDir(dir,fn);
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

static char *GetDKSucInsLocalDir(char *dir,const  FILENAME *fn)
{
	char bankno[BANKNO_LEN+1] = {0};
	char agencyno[AGENCYNO_LEN+1] ={0};
	char temDir[FILE_PATH_CHARNUM] = {0};
	char subId[11] = {0};
	char newDate[9] = { 0 };
	GetGiveDate(newDate);
	memcpy(bankno,fn->BankNo,strlen(fn->BankNo));
	memcpy(agencyno,fn->AgencyNo,strlen(fn->AgencyNo));
	memcpy(subId,fn->PackageId,10);
	sprintf(temDir,"%s/%s/%s/%s/%s/%s/%s/",g_param.FileStoreBasePath,g_param.handleDir
			,bankno,agencyno,newDate,subId,fn->PackageId);
	memcpy(dir,temDir,strlen(temDir));
	JudgeSavePathExist(temDir);
	return dir;
}

int	ReadDKFileName(const char *szFileName, FILENAME *pfilename)
{
	char strFileName[FILE_PATH_CHARNUM] = {0};
	char *pLeftStr = NULL;

	strcpy(strFileName,szFileName);

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
	  memset(pfilename->Date,0,sizeof(pfilename->Date));
	  sprintf(pfilename->Date,"20");
	  memcpy(pfilename->Date+2,pReturn1+8,DATE_LEN-2);
	}
	return 0;
}
