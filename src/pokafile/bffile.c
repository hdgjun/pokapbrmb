#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "pokafile.h"
#include "switch.h"
#include "pokaqueue.h"
#include "db/db.h"
#include "db/dbmonboxaddmon.h"
#include "db/dbbundleinfo.h"

#define  BUNDLEID_NULL_STRING		"XXXXXXXXXXXXXXXXXXXXXXXX"
#define  MACHINEID_NULL_STRING		"XXXXXXXXXXXXXXXXXX"
#define  MONBOXID_NULL_STRING		"XXXXXXXXXXX"

static int ReadBFFileName(const char *szFileName, FILENAME *pfilename);
static int ReadBFFile(FILENAME *fn);
static int MoveBFFile(FILENAME *fn,int result);
static char *GetBFSucInsLocalDir(char *dir,const  FILENAME *fn);

int BFFile(DataType *df)
{
	FILENAME fn;
	int iRet;
	if(df->fileType != BF_FILE_TYPE)
	{
		return WARING;
	}

	vLog("ReadBFFileName[%s]",df->fileName);

	ReadBFFileName(df->fileName,&fn);

	fn.df = df;
	iRet = ReadBFFile(&fn);

	vLog("ReadBFFile[%s:%d]",df->fileName,iRet);

	if(MoveBFFile(&fn,iRet) != SUCESS)
	{
		MoveBFFile(&fn,iRet);//try again
	}
	return iRet;
}

static int ReadBFFile(FILENAME *fn)
{
	char file[FILE_PATH_CHARNUM] = {0};
	FILE *stream;

	sprintf(file,"%s/%s.%s",fn->df->filePath,fn->df->fileName,START_FILE_STRING);

	ADDMONINFO *data;
	ADDMONINFO tem;
	cir_queue_t que;

	stream = fopen(file,ONLYREAD_ACCESS_STRING);
	if(stream == NULL)
	{
		vLog("Open bf file[%s] error[%s]",file,strerror(errno));
		return ERROR;
	}

	init_cir_queue(&que);

	int size;
	while(1)
	{
		memset(&tem,0x00,sizeof(ADDMONINFO));
		size = fread(&tem,sizeof(ADDMONINFO),1,stream);
#ifdef BFNEW
		tem.Id[30]=0x00;
#else
		tem.AddMonDate[19] = 0x00;
#endif
		if(size == 1)
		{
			data = (ADDMONINFO *)malloc(sizeof(ADDMONINFO));
			if(data == NULL)
			{
				 fclose(stream);
				 destroy_cir_queue(&que);
				 vLog("malloc bf file[%s] error[%s]",file,strerror(errno));
				 return WARING;
			}
			memcpy(data,&tem,sizeof(ADDMONINFO));
			data->AddMonChecker[8] = 0x00;
			data->AddMonOperator[8] = 0x00;
			data->BundleId[24] = 0x00;
			data->MachineId[18] = 0x00;
			data->MonBoxId[11] = 0x00;
			data->AddMonDate[19] = 0x00;
			push_cir_queue(&que,data);
#ifdef DEBUG
			vLog("AddMonChecker[%s]",data->AddMonChecker);
			vLog("AddMonDate[%s]",data->AddMonDate);
			vLog("AddMonOperator[%s]",data->AddMonOperator);
			vLog("BundleId[%s]",data->BundleId);
			vLog("MachineId[%s]",data->MachineId);
			vLog("MonBoxId[%s]",data->MonBoxId);
#endif
		}else{
			vLog("strlen(&tem)[%d:%d]",strlen(&tem),(sizeof(ADDMONINFO)-1));
			if(strlen(&tem) == (sizeof(ADDMONINFO)-1)){
				data = (ADDMONINFO *)malloc(sizeof(ADDMONINFO));
				if(data == NULL)
				{
					 fclose(stream);
					 destroy_cir_queue(&que);
					 vLog("malloc bf file[%s] error[%s]",file,strerror(errno));
					 return WARING;
				}
				memcpy(data,&tem,sizeof(ADDMONINFO));
				data->AddMonChecker[8] = 0x00;
				data->AddMonDate[19] = 0x00;
				data->AddMonOperator[8] = 0x00;
				data->BundleId[24] = 0x00;
				data->MachineId[18] = 0x00;
				data->MonBoxId[11] = 0x00;
				push_cir_queue(&que,data);
#ifdef DEBUG
			vLog("AddMonChecker[%s]",data->AddMonChecker);
			vLog("AddMonDate[%s]",data->AddMonDate);
			vLog("AddMonOperator[%s]",data->AddMonOperator);
			vLog("BundleId[%s]",data->BundleId);
			vLog("MachineId[%s]",data->MachineId);
			vLog("MonBoxId[%s]",data->MonBoxId);
#endif

			}else if(strlen(&tem) != 0)
			{//文件格式有错
				 fclose(stream);
				 destroy_cir_queue(&que);
				 vLog(" bf file[%s] format error",file);
				 return ERROR;
			}
			fclose(stream);
			break;
		}
	}
	int rest;
	while(1)
	{
		data = pop_notwait_cir_queue(&que);
		if(data == NULL)
		{
			break;
		}

		if((strcasecmp(data->MachineId,MACHINEID_NULL_STRING) == 0))
		{
			MONBOXADDMON re;
			memset(&re,0x00,sizeof(MONBOXADDMON));
			memcpy(re.scanid,data->BundleId,strlen(data->BundleId));
			memcpy(re.monboxid,data->MonBoxId,strlen(data->MonBoxId));
			memcpy(re.opertime,data->AddMonDate,strlen(data->AddMonDate));
			memcpy(re.operatorid,data->AddMonOperator,strlen(data->AddMonOperator));
			memcpy(re.checker,data->AddMonChecker,strlen(data->AddMonChecker));
			memcpy(re.filename,fn->df->fileName,strlen(fn->df->fileName));
#ifdef BFNEW
			memcpy(re.id,data->Id,strlen(data->Id));
#endif
			rest = DbMonboxaddmon(DBS_INSERT,&re);
			if (rest == WARING)
			{
				if(data)
				free(data);
				destroy_cir_queue(&que);
				return WARING;
			}
		}else
		{
			BUNDLEINFO re;
			memset(&re,0x00,sizeof(BUNDLEINFO));
			if(strcasecmp(data->BundleId, BUNDLEID_NULL_STRING) != 0)
			{
				memcpy(re.bundleid,data->BundleId,strlen(data->BundleId));
				re.flag='0';
			}else
			{
				memcpy(re.bundleid,data->MonBoxId,strlen(data->MonBoxId));
				re.flag='2';
			}
			memcpy(re.percode,data->MachineId,strlen(data->MachineId));
			memcpy(re.scantime,data->AddMonDate,strlen(data->AddMonDate));
			memcpy(re.addmontime,data->AddMonDate,strlen(data->AddMonDate));
			memcpy(re.addmonoperator,data->AddMonOperator,strlen(data->AddMonOperator));
			memcpy(re.addmonchecker,data->AddMonChecker,strlen(data->AddMonChecker));
			memcpy(re.filename,fn->df->fileName,strlen(fn->df->fileName));
#ifdef BFNEW
			memcpy(re.id,data->Id,strlen(data->Id));
#endif
			memcpy(re.bankno,fn->BankNo,strlen(fn->BankNo));
			memcpy(re.agencyno,fn->AgencyNo,strlen(fn->AgencyNo));

			rest = DbBundleinfo(DBS_INSERT,&re);
			if (rest == WARING)
			{
				if(data)free(data);
				destroy_cir_queue(&que);
				DbsRollback();
				return WARING;
			}

		}

		if(data)free(data);
	}
	rest = DbsCommit();
	destroy_cir_queue(&que);
	return rest;
}
static int MoveBFFile(FILENAME *fn,int result)
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
			GetBFSucInsLocalDir(dir,fn);
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

static char *GetBFSucInsLocalDir(char *dir,const  FILENAME *fn)
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

static int ReadBFFileName(const char *szFileName, FILENAME *pfilename)
{
		char strFileName[FILE_PATH_CHARNUM] = { 0 };
		char *pLeftStr = NULL;
		char *pReturn1;

		memset(pfilename,0x00,sizeof(FILENAME));

		strcpy(strFileName, szFileName);

		/*获取银行号*/
		pReturn1 = strtok_r(strFileName, SEPARATOR_STRING, &pLeftStr);
		if ((pReturn1 != NULL) && (BANKNO_LEN == strlen(pReturn1))) {
				memcpy(pfilename->BankNo, pReturn1, BANKNO_LEN);
		}
		/*获取网点号*/
		pReturn1 = strtok_r(NULL, SEPARATOR_STRING, &pLeftStr);
		if ((pReturn1 != NULL) && (AGENCYNO_LEN == strlen(pReturn1))) {
				memcpy(pfilename->AgencyNo, pReturn1, AGENCYNO_LEN);
		}

		/*获取盒子号*/
		pReturn1 = strtok_r(NULL, SEPARATOR_STRING, &pLeftStr);
		if ((pReturn1 != NULL) && (BOXNO_LEN == strlen(pReturn1))) {
				memcpy(pfilename->BoxNo, pReturn1, BOXNO_LEN);
		}

		/*获取日期*/
		pReturn1 = strtok_r(NULL, SEPARATOR_STRING, &pLeftStr);
		if (pReturn1 != NULL) {
		if (strstr(pReturn1, TIMEFILL_STRING) == 0) {/*盒子文件获取日期*/
				memcpy(pfilename->Date, pReturn1, DATE_LEN);
		} else {/*采集软件获取日期*/
				memcpy(pfilename->Date, pReturn1 + 7, DATE_LEN);
		}
		}
		return SUCESS;
}
