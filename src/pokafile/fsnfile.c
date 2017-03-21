#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "db/db.h"
#include "pokafile.h"
#include "switch.h"
#include "db/dbmoneydataatm.h"
#include "db/dbbusinesslistatm.h"
#include "db/dbperinfo.h"
#include "db/dbmoneydata.h"
#include "db/dbkunfiles.h"
#include "common.h"
#include "monrule.h"

#ifdef DEBUG
	#include <time.h>
#endif

int ReadSnoNum(char szReadBuf[FILE_HEAD_SIZE]);
int ReadSNoInfoFromFSNFile(char *list,int iNumSNoFSN,FILENAME *pfilename);
void RestoreDateTime(FILERECORD *FileRecord,int DateArray,int DateArray1,int TimeArray,int TimeArray1);
int ReadOneRecordInfo(FILENAME *pfilename,FILERECORD *FileRecord,char *list,int iSnoNo);
ATMRECODE *GetAtmRecode(ATMRECODE *atm,FILERECORD *recode,FILENAME *fn);
MONEYDATAR *GetFSNRecode(MONEYDATAR *fsn,FILERECORD *recode,FILENAME *fn);

char *GtmpTableName(char *tab,int id);
int MoveATMFSNFile(FILENAME *fn,int result);

int FSNFile(DataType *df)
{
	FILENAME fn ;
	ATMBUSINESSLIST business;
	int iRet;
	vLog("filename[%d]:%s",df->fileType,df->fileName);
	if(df->fileType != FSN_FILE_TYPE){
		return WARING;
	}

	if(strstr(df->fileName, ATM_FILE_STRING) != 0){
		df->fileType = ATM_FILE_TYPE;
	}
	if(strstr(df->fileName, CRS_FILE_STRING) != 0){
		df->fileType = CRS_FILE_TYPE;
	}

#ifdef DEBUG
	double start, finish;
	start = clock();//取开始时间
	vLog("START_FSNFILE:%s/%s type:%d",df->filePath,df->fileName,df->fileType);
#endif
	memset(&fn,0x00,sizeof(FILENAME));
	char tn[100] = {0};
    if(df->fileType == FSN_FILE_TYPE)
    {
		ReadFSNOrBKFileName(df->fileName,&fn);
    }else{
    	/*创建moneydata_atm临时表*/
#ifdef DEBUG
    	vLog("create temp table");
#endif
    	GtmpTableName(tn,df->threadid);
    	if(TmpDbsMoneydataAtm(DBS_SELECT,tn,NULL)!=SUCESS)
    	{
    		vLog("create temp table [%s]",tn);
			if(TmpDbsMoneydataAtm(DBS_CREATE_TEMP,tn,NULL)!=SUCESS)
			{
				return WARING;
			}
    	}
#ifdef DEBUG
        vLog("after create temp table %s",tn);
#endif
    	iRet = ReadAtmFileName(df->fileName,&fn,&business);
    	if(iRet == ERROR){
    		fn.df = df;
    		MoveFSNFile(&fn,iRet);
    		return ERROR;
    	}

    	iRet = DbBusinessListAtm(DBS_SELECT1,&business);
		if(iRet != SUCESS)
		{//重复数据
			iRet = ERROR;
			 vLog("Duplicate business data [%d] ",business.id);
			fn.df = df;
			MoveFSNFile(&fn,iRet);
			return ERROR;
		}

    }

	fn.df = df;
    /*read fsn file*/
	iRet = WriteToSnoTable(&fn);

	if(df->fileType == FSN_FILE_TYPE)
	{
	}else
	{
		if(iRet == SUCESS)
		{
			while(1){
				iRet = DbBusinessListAtm(DBS_INSERT,&business);
				if(iRet != SUCESS){
					vLog("Insert into table businesslistatm err!");
					break;
				}
				iRet = DbBusinessListAtm(DBS_SEL_ONE_ROW,&business);
				if(iRet != SUCESS)
				{
					vLog("Select Business id err!");
					break;
				}

				iRet = FromTmpToAtm(tn,business.id);

				vLog("FromTmpToAtm[%d]!",iRet);

				if(iRet == SUCESS){
					iRet = DbsCommit();
				}else{
					DbBusinessListAtm(DBS_DELETE,&business);
					DbsCommit();
				}
				break;
			}
		}
		TmpDbsMoneydataAtm(DBS_TRUNCATE,tn,NULL);
	}

	if(MoveFSNFile(&fn,iRet) != SUCESS)
	{
		MoveFSNFile(&fn,iRet);//try again
	}
#ifdef DEBUG
	finish = clock();//取结束时间
	vLog("FINSH_FSNFILE[%d]:%s/%d",df->filePath,df->fileName,(finish-start));
#endif
	return iRet;
}
#ifndef PEOPLEBAK
char *GetPercode(const char *src,char *des){
	int len = strlen(src);
	int i;
	for(i=0;i<len;i++){
		if(src[i] == '/'){
			des[i] = '!';
		}else{
			des[i] = src[i];
		}
	}
	return des;
}
#else
char *GetPercode(const char *src,char *des){
	int len = strlen(src);
	int i;
	for(i=0;i<len;i++){
		if(src[i] == '!'){
			des[i] = '/';
		}else{
			des[i] = src[i];
		}
	}
	return des;
}
#endif
int MoveATMFSNFile(FILENAME *fn,int result)
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
			GetSucInsLocalDir(dir,fn);
#ifdef PEOPLEBANK
			sprintf(fe.LocalDesBaseName,"%s",fn->df->fileName);

#else
			char percode[50] = {0};
			GetPercode(fn->Percode,percode);
			char *sub = strstr(fn->df->fileName,ATM_SEPARATOR_STRING);
#ifdef DEBUG
			vLog("bankno[%s] agencyno[%s] percode[%s] sub[%s]",fn->BankNo,fn->AgencyNo,fn->Percode,sub);
#endif
			memset(fe.LocalDesBaseName,0x00,MAX_STRING_SIZE);
			sprintf(fe.LocalDesBaseName,"%s_%s_%s_%s",
					fn->BankNo,fn->AgencyNo,percode,
					sub);
			memcpy(fe.LocalDesFilePath,dir,strlen(dir));
#endif

#ifdef DEBUG
			vLog("LocalDesBaseName[%s]",fe.LocalDesBaseName);
			vLog("LocalDesFilePath[%s]",fe.LocalDesFilePath);
#endif
		break;
		case WARING:return result;
	}
	sprintf(fe.FileBaseName,"%s.%s",fn->df->fileName,START_FILE_STRING);
	memcpy(fe.FilePth,fn->df->filePath,strlen(fn->df->filePath));
	fe.isRemote = 0;
	return MoveFiles(&fe);
}

int MoveFSNFile(FILENAME *fn,int result)
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
			GetSucInsLocalDir(dir,fn);
			sprintf(fe.LocalDesBaseName,"%s",fn->df->fileName);
			memcpy(fe.LocalDesFilePath,dir,strlen(dir));
			char c = *(fn->PackageId+9);
			if(isalpha(c))
			{
				TKUNFILES data;
				memset(&data,0x00,sizeof(TKUNFILES));
				memcpy(data.bundleCode,fn->PackageId,strlen(fn->PackageId));
				memcpy(data.path,dir,strlen(dir));
				memcpy(data.name,fn->df->fileName,strlen(fn->df->fileName));

				if(fn->df->fileType == BK_FILE_TYPE)
				{
					sprintf(data.type,"%s",BK_FILE_STRING);
				}else
				{
					sprintf(data.type,"%s",FSN_FILE_STRING);
				}
				DbKunFiles(DBS_INSERT,&data);
				DbsCommit();
			}
			break;
		case WARING:return result;
	}

	sprintf(fe.FileBaseName,"%s.%s",fn->df->fileName,START_FILE_STRING);
	memcpy(fe.FilePth,fn->df->filePath,strlen(fn->df->filePath));
	fe.isRemote = 0;
	return MoveFiles(&fe);
}

char *GetSucInsLocalDir(char *dir,const  FILENAME *fn)
{
	char bankno[BANKNO_LEN+1] = {0};
	char agencyno[AGENCYNO_LEN+1] ={0};
	char temDir[FILE_PATH_CHARNUM] = {0};
	char subId[11] = {0};
	char newDate[9] = { 0 };
	GetGiveDate(newDate);
#ifdef PEOPLEBANK
	if(CheckSortCenter(fn->BankNo) == SUCESS){
		memcpy(bankno,fn->AgencyNo,strlen(fn->AgencyNo));
	}else{
		memcpy(bankno,fn->BankNo,strlen(fn->BankNo));
	}
#else
	memcpy(bankno,fn->BankNo,strlen(fn->BankNo));
#endif
	memcpy(agencyno,fn->AgencyNo,strlen(fn->AgencyNo));

	if(fn->df->fileType == FSN_FILE_TYPE||fn->df->fileType == BK_FILE_TYPE)
	{
		char c = *(fn->PackageId+9);
		if(isalpha(c)){
			sprintf(subId,"%s",PACKAGE_STRING);
		}else{
			memcpy(subId,fn->PackageId,10);
		}
		sprintf(temDir,"%s/%s/%s/%s/%s/%s/%s/",g_param.FileStoreBasePath,g_param.handleDir
								,bankno,agencyno,newDate,subId,fn->PackageId);
	}else{
		sprintf(temDir,"%s/%s/%s/%s/%s/%s/",g_param.FileStoreBasePath,g_param.handleDir
							,bankno,agencyno,newDate,fn->Percode);
	}
	memcpy(dir,temDir,strlen(temDir));
	JudgeSavePathExist(temDir);
	return dir;
}

int ATMFile(DataType *df)
{
    return SUCESS;
}
int CheckFSNRecode(FILERECORD *fileRecord)
{
	return SUCESS;
}
void CheckDoubtMon(MONEYDATAR *rec)
{
	int j = 0;
	int k = 0;
	int imatchflag = 0;

	MONRULEARRY *rule;
	rule = getMonrule();
	if(rule==NULL)
	{
		vLog("get doubt mon rule error");
		return;
	}
	for(j = 0; j < rule->size; j++)
	{
		imatchflag = 0;//每比对一次完成置回初始值

		//比对版别、币值、冠字号
		printf("ReadOneRecordInfo FileRecord->Ver:%s MoneyRule[%d].MonVer:%s\n",rec->monver,
																					j,
																					rule->data[j].MonVer);

		if(strlen(rule->data[j].MonVer)>1)
		{
			if(0 != strcmp(rec->monver,rule->data[j].MonVer))
			{
				continue;
			}
		}
		printf("ReadOneRecordInfo FileRecord->Valuta:%s MoneyRule[%d].MonVer:%s\n",rec->monval,
																					j,
																					rule->data[j].MonVal);
		if(0 != strcmp(rec->monval,rule->data[j].MonVal))
		{
			continue;
		}

		int iCharNum = SNO_LEN-2;
		for(k = 0; k < iCharNum; k++)
		{
			printf("ReadOneRecordInfo FileRecord->SNo[%d]:%c pfilename->MoneyRule[%d].MonHead[%d]:%c\n",k,
																					rec->mon[k],
																					j,
																					k,
																					rule->data[j].MonHead[k]);
			if((MATCH_CHAR != rule->data[j].MonHead[k]) &&
				(rec->mon[k] != rule->data[j].MonHead[k]))
			{
				imatchflag = -1;
				break;
			}
		}

		if(0 == imatchflag)
		{
			DbsDoubtMoneydata(DBS_INSERT,rec);
			break;
		}
	}
}

int	WriteToSnoTable(FILENAME *fsnFile)
{
#ifdef DEBUG
	vLog("READ_FSNFILE_HEAD:%s/%s",fsnFile->df->filePath,fsnFile->df->fileName);
#endif

	char strAllFilePath[FILE_PATH_CHARNUM] = {0};

	sprintf(strAllFilePath,"%s/%s.%s",fsnFile->df->filePath,fsnFile->df->fileName,START_FILE_STRING);

	FILE *pfile;
	pfile = fopen(strAllFilePath,"r");//只读打开???防止意外写入
	if(pfile == NULL)
	{
		vLog("Can't open fsn file error:%s/%s",fsnFile->df->filePath,fsnFile->df->fileName);
		return ERROR;
	}

	char FileHeadBuf[FILE_HEAD_SIZE] = {0};//读文件头
	int ifileheadread = fread(FileHeadBuf,sizeof(char),sizeof(FileHeadBuf),pfile);
	if(ifileheadread != FILE_HEAD_SIZE)
	{
		printf("Read file head size fail:%d\n",ifileheadread);
		vLog("Read file head size fail:%d",ifileheadread);
		fclose(pfile);
		pfile = NULL;
		return ERROR;
	}

	if(SUCESS == JudgeIncludeImageSno(FileHeadBuf))//判断是否包含图像
	{
		fsnFile->IncludeImageSnoFlag = 1;
	}
	else
	{
		fsnFile->IncludeImageSnoFlag = 0;
	}


	unsigned int iFSNSNoNum = 0;//冠字号记录数
	iFSNSNoNum = ReadSnoNum(FileHeadBuf);//冠字号记录数

	if(MAX_ONE_FSN_SIZE < iFSNSNoNum)
	{

		vLog("FSN file too large:%s:%ld",fsnFile->df->fileName,iFSNSNoNum);
		fclose(pfile);
		pfile = NULL;
		return ERROR;
	}
	long lReadBufSize = 0;
	if((FILE_HEAD_SIZE + STANDARD_ONE_RECORD_SIZE*iFSNSNoNum) == fsnFile->df->filesize)	//标准FSN
	{
		lReadBufSize = FILE_HEAD_SIZE + STANDARD_ONE_RECORD_SIZE*iFSNSNoNum;
		fsnFile->OneRecordSize = STANDARD_ONE_RECORD_SIZE;
	}
	else if((FILE_HEAD_SIZE + ONE_RECORD_SIZE*iFSNSNoNum) == fsnFile->df->filesize)		//Poka.FSN
	{
		lReadBufSize = FILE_HEAD_SIZE + ONE_RECORD_SIZE*iFSNSNoNum;
		fsnFile->OneRecordSize = ONE_RECORD_SIZE;
	}else
	{
		vLog("File[%s] size error,file size:%ld,standard:%ld,poka:%ld"
				,fsnFile->df->fileName
				,fsnFile->df->filesize
				,(FILE_HEAD_SIZE + STANDARD_ONE_RECORD_SIZE*iFSNSNoNum)
				,(FILE_HEAD_SIZE + ONE_RECORD_SIZE*iFSNSNoNum));
		fclose(pfile);
		pfile = NULL;
		return ERROR;
	}

	char *readBuf = (char *)malloc(lReadBufSize);
	fseek(pfile,0,SEEK_SET);//文件指针移至文件头
	long iallread = fread(readBuf,sizeof(char),lReadBufSize,pfile);

	if(iallread != lReadBufSize)
	{
		free(readBuf);//释放
		readBuf = NULL;//指针置为NULL
		if(ferror(pfile)){
			vLog("Read error[%s];file size:%ld,read size:%ld,error:%ld"
							,fsnFile->df->fileName
							,fsnFile->df->filesize
							,iallread,ferror(pfile));
			fclose(pfile);
			pfile = NULL;
			return WARING;
		}
		fclose(pfile);
		pfile = NULL;
		return ERROR;
	}

	fclose(pfile);
	pfile = NULL;

	int iReadRet = ReadSNoInfoFromFSNFile(readBuf,iFSNSNoNum,fsnFile);//读取冠字号信息

	free(readBuf);//释放
	readBuf = NULL;//指针置为NULL

	return iReadRet;
}

int ReadSNoInfoFromFSNFile(char *list,int iNumSNoFSN,FILENAME *pfilename)
{
	if(list == NULL)
	{
		printf("Read fsn file buf is null\n");
		vLog("Read fsn file buf is null");
		return WARING;
	}

    int iSnoCountFSN;
    int iRet;
    int err=0;
    int suc=0;
    char tmpTable[20];
	for(iSnoCountFSN = 0; iSnoCountFSN < iNumSNoFSN; iSnoCountFSN++)
	{
		FILERECORD fileRecord;
		memset(&fileRecord,0,sizeof(FILERECORD));
		//读取一条冠字号全部信息
		if(ReadOneRecordInfo(pfilename,&fileRecord,list,iSnoCountFSN) == SUCESS)
		{
			//iRet = CheckFSNRecode(&fileRecord);
			if(pfilename->df->fileType == FSN_FILE_TYPE)
			{
				MONEYDATAR rec;
				GetFSNRecode(&rec,&fileRecord,pfilename);
				iRet = DbsMoneydata(DBS_INSERT,&rec);
				CheckDoubtMon(&rec); //黑名单
			}else
			{
				ATMRECODE atm;
				GetAtmRecode(&atm,&fileRecord,pfilename);
				iRet = TmpDbsMoneydataAtm(DBS_INSERT,GtmpTableName(tmpTable,pfilename->df->threadid),&atm);
				vLog("TmpDbsMoneydataAtm[%d]",iRet);
			}
			if(iRet == WARING)
			{ /*没有连接数据库*/
				vLog("DbsRollback");
				DbsRollback();
				return iRet;
			}else if(iRet == ERROR)
			{
				err++;
			}else{
				suc++;
			}
		}else{
			vLog("WARING");
			return WARING;
		}

	}
	int ab = DbsCommit();
	vLog("Insert file[%s/%s],sucess[%d],err[%d]",pfilename->df->filePath,pfilename->df->fileName,suc,err);
	return ab;
}

int ReadOneRecordInfo(FILENAME *pfilename,FILERECORD *FileRecord,char *list,int iSnoNo)
{
	if(list == NULL)
	{
		printf("Fsn file read buf is null\n");
		vLog("Fsn file read buf is null");
		return ERROR;
	}
	int loc = iSnoNo*pfilename->OneRecordSize;

#ifdef TIME_FLAG
	memset(FileRecord->DateTime,0x00,sizeof(FileRecord->DateTime));
	sprintf(FileRecord->DateTime,"%c%c%c%c-%c%c-%c%c %c%c:%c%c:%c%c ",pfilename->DateTime[0],pfilename->DateTime[1]
													,pfilename->DateTime[2],pfilename->DateTime[3]
												    ,pfilename->DateTime[4],pfilename->DateTime[5]
													,pfilename->DateTime[6],pfilename->DateTime[7]
													,pfilename->DateTime[8],pfilename->DateTime[9]
													,pfilename->DateTime[10],pfilename->DateTime[11]
													,pfilename->DateTime[12],pfilename->DateTime[13]);


#else
	//无符号整型转化成字符串(原来是字符直接串接即可)
		//日期
		int iDateArray[2];
		iDateArray[0] = list[loc + 32];
		iDateArray[1] = list[loc + 33];
	//时间
		int iTimeArray[2];
		iTimeArray[0] = list[loc + 34];
		iTimeArray[1] = list[loc + 35];

		RestoreDateTime(FileRecord,iDateArray[0],iDateArray[1],iTimeArray[0],iTimeArray[1]);//解析日期时间

		char strSnoTime[10] = {0};
		memset(strSnoTime,0,sizeof(strSnoTime));//加入因为秒数变3位
		strcpy(strSnoTime,FileRecord->Time);
		sprintf(FileRecord->DateTime,"%s %s",FileRecord->Date,strSnoTime);

#endif
	//真假币标志
	sprintf(FileRecord->tfFlag,"%d",list[loc + 36]);//"1"

	//假币特征值
	sprintf(FileRecord->ErrorCode,"%d",list[loc + 38]);//"0"

	//币种
	unsigned int i = 0;

	for(i = 0; i < 4; i++)
	{
		FileRecord->MoneyFlag[i] = list[loc + 44 + 2*i];
	}

	//版别（0 1 2代表1990 1999 2005版）
	char strVer[10] = {0};
	sprintf(strVer,"%x%x",(unsigned char)(list[loc + + 53]),(unsigned char)(list[loc + + 52]));//"2"
	int iVer = -1;
	iVer = htoi(strVer);
	switch(iVer)
	{
		case 0:
			strcpy(FileRecord->Ver,MONVER_01_STRING);
			break;
		case 1:
			strcpy(FileRecord->Ver,MONVER_02_STRING);
			break;
		case 2:
			strcpy(FileRecord->Ver,MONVER_03_STRING);
			break;
		case 3:
			strcpy(FileRecord->Ver,MONVER_04_STRING);
			break;
		case 1990:
		case 1999:
		case 2005:
		case 2015:
			sprintf(FileRecord->Ver,"%d",iVer);
			break;
		default:
			printf("Version error:%d\n",iVer);
			vLog("Version error:%d",iVer);
			break;
	}

	//币值
	sprintf(FileRecord->Valuta,"%d",list[loc + 54]);//"100"

	//冠字号字符个数
	int iCharNum = list[loc + 56];//"10"

	printf("---->iCharNum:%d\n",iCharNum);
	if(((SNO_LEN-2) != iCharNum) &&
		(SNO_LEN != iCharNum))
	{
		iCharNum = (SNO_LEN-2);
	}

	int itest = 0;
	//冠字号
	for(i = 0; i < iCharNum; i++)
	{
		itest = strlen(&list[loc + 58 + 2*i]);
		printf("1itest:%d\n",itest);
		if(1 == itest)
		{
			FileRecord->SNo[i] = list[loc + 58 + 2*i];
		}
	}
	printf("ReadOneRecordInfo FileRecord.SNo:%s/\n",FileRecord->SNo);
	strtrim(FileRecord->SNo);
	FileRecord->SNo[12]=0x00;
	//机器（具）编号
	for(i = 0; i < 24; i++)
	{
		FileRecord->MachineSNo[i] = list[loc + 82 + 2*i];
	}
	printf("FileRecord->MachineSNo:%s/\n",FileRecord->MachineSNo);

	FileRecord->MachineSNo[24]=0x00;

	FileRecord->Reserve1[0] = list[loc + 130];
	printf("FileRecord->Reserve1:%c\n",FileRecord->Reserve1[0]);

	if(g_param.SaveImage==DEF_IMAGE_SAVE)
	{
		if(1 == pfilename->IncludeImageSnoFlag)
		{
			char ImageFileName[MAX_STRING_SIZE] = {0};
			char strDateTime[20] = {0};
			char sDateTime[20] = {0};

			sprintf(strDateTime,"%s",pfilename->DateTime);
			if(strlen(FileRecord->SNo) == 0)
			{
				vLog("mon is null");
				return ERROR;
			}

			sprintf(ImageFileName,"%s_%d%d_%d.bmp",FileRecord->SNo,GetDateInt(),GetTimeInt(),iSnoNo);


			char FolderPath[FILE_PATH_CHARNUM] = {0};//文件夹路径
			sprintf(FolderPath,"%s/%s/%s/%s/",g_param.ImagePath,g_param.ImageDir,pfilename->BankNo,pfilename->Date);

			JudgeSavePathExist(FolderPath);

			sprintf(FileRecord->ImageFilePath,"%s/%s",FolderPath,ImageFileName);

			SaveSnoImageFromFsnFile(pfilename,FileRecord,list,iSnoNo);//截取图片
		}
	}

	unsigned int uiBusinessType = 0;
	if(STANDARD_ONE_RECORD_SIZE == pfilename->OneRecordSize)
	{
		memcpy(FileRecord->BusinessType,pfilename->BusinessType,BUSINESSTYPE_LEN);
	}else
	{
		unsigned int uiSpace = 1676;
		//打捆业务
		//捆条码
		for(i = 0; i < 24; i++)
		{
			FileRecord->BundleID[i] = list[loc + uiSpace + i];
		}
		printf("----------->BundleID:%s\n",FileRecord->BundleID);

		//操作员
		for(i = 0; i < 8; i++)
		{
			FileRecord->Operator[i] = list[loc + uiSpace+24 + i];
		}

		//监督员
		for(i = 0; i < 8; i++)
		{
			FileRecord->Monitor[i] = list[loc + uiSpace+32 + i];
		}
		//printf("----------->MonitorID:%s\n",FileRecord->Monitor);

		//钞箱ID
		for(i = 0; i < 24; i++)
		{
			FileRecord->MonBoxID[i] = list[loc + uiSpace+40 + i];
		}
		//printf("----------->MonBoxID:%s\n",FileRecord->MonBoxID);

		//ATM机ID
		for(i = 0; i < 24; i++)
		{
			FileRecord->AtmID[i] = list[loc + uiSpace+64 + i];
		}
		//printf("----------->AtmID:%s\n",FileRecord->AtmID);

		//加钞员
		for(i = 0; i < 8; i++)
		{
			FileRecord->AddMonOperator[i] = list[loc + uiSpace+88 + i];
		}
		//printf("----------->AddMonOperator:%s\n",FileRecord->AddMonOperator);

		//加钞监管员
		for(i = 0; i < 8; i++)
		{
			FileRecord->AddMonChecker[i] = list[loc + uiSpace+96 + i];
		}

			//业务类型(存取款标志)
		uiBusinessType = list[loc + uiSpace+104];
		itoafunc(uiBusinessType,FileRecord->BusinessType);
	}

	if(3 == uiBusinessType)
	{
		strcpy(FileRecord->Operator,FileRecord->AddMonOperator);
		strcpy(FileRecord->Monitor,FileRecord->AddMonChecker);
	}
	else
	{
		memset(FileRecord->MonBoxID,0,sizeof(char)*25);//添加因为钞箱ID乱码插入报错
		memset(FileRecord->AtmID,0,sizeof(char)*25);
		memset(FileRecord->AddMonOperator,0,sizeof(char)*9);
		memset(FileRecord->AddMonChecker,0,sizeof(char)*9);
	}


	sprintf(FileRecord->BankNo,"%s",pfilename->BankNo);
	sprintf(FileRecord->AgencyNo,"%s",pfilename->AgencyNo);
	sprintf(FileRecord->FileName,"%s",pfilename->df->fileName);

	memcpy(FileRecord->PackageId,pfilename->PackageId,KUNID_LEN);

#ifdef DEBUG
/*	vLog("------>FileRecord->MachineSNo:%s\n",FileRecord->MachineSNo);
	vLog("------>strDateTime:%s\n",FileRecord->DateTime);
	vLog("------>FileRecord->SNo:%s\n",FileRecord->SNo);
	vLog("------>FileRecord->MoneyFlag:%s\n",FileRecord->MoneyFlag);
	vLog("------>FileRecord->Valuta:%s\n",FileRecord->Valuta);
	vLog("------>FileRecord->Ver:%s\n",FileRecord->Ver);
	vLog("------>FileRecord->tfFlag:%s\n",FileRecord->tfFlag);
	vLog("------>FileRecord->Operator:%s\n",FileRecord->Operator);
	vLog("------>FileRecord->BundleID:%s\n",FileRecord->BundleID);
	vLog("------>FileRecord->Monitor:%s\n",FileRecord->Monitor);
	vLog("------>FileRecord->BusinessType:%s\n",FileRecord->BusinessType);
	vLog("------>FileRecord->MonBoxID:%s\n",FileRecord->MonBoxID);
	vLog("------>FileRecord->AtmID:%s\n",FileRecord->AtmID);
	vLog("------>FileRecord->AddMonOperator:%s\n",FileRecord->AddMonOperator);
	vLog("------>FileRecord->AddMonChecker:%s\n",FileRecord->AddMonChecker);
	*/
#endif
	return SUCESS;
}

void RestoreDateTime(FILERECORD *FileRecord,int DateArray,int DateArray1,int TimeArray,int TimeArray1)
{
	int iYear = (DateArray1>>1) + 1980;
	int iMonth = ((DateArray1<<3)&0x0008)+((DateArray>>5)&0x0007);
	int iDay = DateArray&0x001F;

	if((iMonth >= 1) && (iMonth <= 12) &&
	   (iDay >= 1) && (iDay <= 31))
	{
		sprintf(FileRecord->Date,"%d-%02d-%02d",iYear,iMonth,iDay);
                printf("date = %s\n",FileRecord->Date);
	}
	else
	{
		printf("Month or day error:%d/%d\n",iMonth,iDay);
		vLog("Month or day error:%d/%d",iMonth,iDay);
	}

	int iHour = ((TimeArray1>>3)&0x001F);
	int iMinute = ((TimeArray1<<3)&0x0038) + ((TimeArray>>5)&0x0007);
	int iSecond = (TimeArray&0x001F)<<1;

	if((iHour >= 0) && (iHour <= 23) &&
	   (iMinute >= 0) && (iMinute <= 59) &&
	   (iSecond >= 0) && (iSecond <= 59))
	{
		sprintf(FileRecord->Time,"%02d:%02d:%02d",iHour,iMinute,iSecond);
	}
	else
	{
		printf("Hour or minute or second error\n");
		vLog("Hour or minute or second error %02d:%02d:%02d",iHour,iMinute,iSecond);
	}
}

char *GtmpTableName(char *tab,int id)
{
	char ta[100] = {0};
	sprintf(ta,"MONEYDATA_ATM_%d",id);
	memcpy(tab,ta,strlen(ta));
	return tab;
}

MONEYDATAR *GetFSNRecode(MONEYDATAR *fsn,FILERECORD *recode,FILENAME *fn)
{
	memset(fsn,0x00,sizeof(MONEYDATAR));
	memcpy(fsn->percode,recode->MachineSNo,strlen(recode->MachineSNo));
	memcpy(fsn->coltime,recode->DateTime,strlen(recode->DateTime));
	memcpy(fsn->mon,recode->SNo,strlen(recode->SNo));
	memcpy(fsn->montype,recode->MoneyFlag,strlen(recode->MoneyFlag));
	memcpy(fsn->monval,recode->Valuta,strlen(recode->Valuta));
	memcpy(fsn->monver,recode->Ver,strlen(recode->Ver));
	fsn->trueflag=recode->tfFlag[0];
	memcpy(fsn->operatorid,recode->Operator,strlen(recode->Operator));
	memcpy(fsn->bundleid,recode->BundleID,strlen(recode->BundleID));
	memcpy(fsn->checker,recode->Monitor,strlen(recode->Monitor));

	memcpy(fsn->imagepath,recode->ImageFilePath,strlen(recode->ImageFilePath));
	memcpy(fsn->businesstype,fn->BusinessType,strlen(fn->BusinessType));

	memcpy(fsn->monboxid,recode->MonBoxID,strlen(recode->MonBoxID));
	memcpy(fsn->atmid,recode->AtmID,strlen(recode->AtmID));
	memcpy(fsn->addmonoperator,recode->AddMonOperator,strlen(recode->AddMonOperator));
	memcpy(fsn->addmonchecker,recode->AddMonChecker,strlen(recode->AddMonChecker));

	memcpy(fsn->bankno,fn->BankNo,strlen(fn->BankNo));
	memcpy(fsn->agencyno,fn->AgencyNo,strlen(fn->AgencyNo));
	memcpy(fsn->packageid,recode->PackageId,strlen(recode->PackageId));
	memcpy(fsn->filename,recode->FileName,strlen(recode->FileName));
	memcpy(fsn->imagepath,recode->ImageFilePath,strlen(recode->ImageFilePath));
	return fsn;
}
ATMRECODE *GetAtmRecode(ATMRECODE *atm,FILERECORD *recode,FILENAME *fn)
{
	memset(atm,0x00,sizeof(ATMRECODE));
	memcpy(atm->percode,fn->Percode,strlen(fn->Percode));
	memcpy(atm->coltime,fn->DateTime,strlen(fn->DateTime));
	memcpy(atm->mon,recode->SNo,strlen(recode->SNo));
	memcpy(atm->montype,recode->MoneyFlag,strlen(recode->MoneyFlag));
	memcpy(atm->monval,recode->Valuta,strlen(recode->Valuta));
	memcpy(atm->monver,recode->Ver,strlen(recode->Ver));
	atm->trueflag=recode->tfFlag[0];
	memcpy(atm->imagepath,recode->ImageFilePath,strlen(recode->ImageFilePath));
	memcpy(atm->businesstype,fn->BusinessType,strlen(fn->BusinessType));
	memcpy(atm->bankno,fn->BankNo,strlen(fn->BankNo));
	memcpy(atm->agencyno,fn->AgencyNo,strlen(fn->AgencyNo));
	return atm;
}

int GetMessageFromPerInfo(char *src,FILENAME *fn)
{
	char tem[FILE_PATH_CHARNUM] = { 0 };
	int iret;
	if(src)
	{
#ifndef PEOPLEBANK
		PERINFO record;
		memset(&record,0x00,sizeof(PERINFO));
		memcpy(record.terminalno,src,strlen(src));
		vLog("terminalno:%s",record.terminalno);
		if((iret=DbPerinfo(DBS_SELECT,&record))!=SUCESS)
			return iret;
		memcpy(fn->BankNo,record.bankno,strlen(record.bankno));
		memcpy(fn->AgencyNo,record.agencyno,strlen(record.agencyno));
		memcpy(fn->Percode,record.percode,strlen(record.percode));
		return SUCESS;
#endif
#ifdef PEOPLEBANK
		char *pLeftStr = NULL;
		char *pReturn1;
		/*bankno*/
		pReturn1 = strtok_r(tem, SEPARATOR_STRING, &pLeftStr);
		if (pReturn1 != NULL)
		{
			memcpy(fn->BankNo,pReturn1,strlen(pReturn1));
		}else{
			return ERROR;
		}
		/*agencyno*/
		pReturn1 = strtok_r(NULL, SEPARATOR_STRING, &pLeftStr);
		if (pReturn1 != NULL)
		{
			memcpy(fn->AgencyNo,pReturn1,strlen(pReturn1));
		}else{
			return ERROR;
		}

		/*Percode*/
		pReturn1 = strtok_r(NULL, SEPARATOR_STRING, &pLeftStr);
		if (pReturn1 != NULL)
		{
			char percode[50] = {0};
			GetPercode(pReturn1,fn->Percode);
		}else{
			return ERROR;
		}

#endif
	}
	return ERROR;
}
char *DeleteXOrx(char *des,char *src)
{
	char *pt;
	pt=src;
	int i=0;
	while(1)
	{
		if(*pt=='X'||*pt=='x'){
			i++;
			pt++;
			continue;
		}
		break;
	}
	memcpy(des,src+i,strlen(src)-i);
	return des;
}
int ReadAtmFileName(const char *szFileName, FILENAME *fn,ATMBUSINESSLIST *business)
{
	char strFileName[FILE_PATH_CHARNUM] = { 0 };
	char *pLeftStr = NULL;
	char *pReturn1;

	strcpy(strFileName, szFileName);

	/*获取银行号*/
	pReturn1 = strtok_r(strFileName, ATM_SEPARATOR_STRING, &pLeftStr);
	if (pReturn1 != NULL)
	{
		if(GetMessageFromPerInfo(pReturn1,fn)!=SUCESS)
		{
			vLog("GetMessageFromPerInfo error");
			return ERROR;
		}
		memcpy(business->bankno,fn->BankNo,strlen(fn->BankNo));
		memcpy(business->netno,fn->AgencyNo,strlen(fn->AgencyNo));
		memcpy(business->percode,fn->Percode,strlen(fn->Percode));

	}else{
		return ERROR;
	}

	pReturn1 = strtok_r(NULL, ATM_SEPARATOR_STRING, &pLeftStr);
	if (pReturn1 != NULL)
	{

	}else{
		return ERROR;
	}
	/*业务流水号*/
	pReturn1 = strtok_r(NULL, ATM_SEPARATOR_STRING, &pLeftStr);
	if (pReturn1 != NULL)
	{
		DeleteXOrx(business->businessid,pReturn1);
	}else{
		return ERROR;
	}
	/*卡号*/
	pReturn1 = strtok_r(NULL, ATM_SEPARATOR_STRING, &pLeftStr);
	if (pReturn1 != NULL)
	{
		DeleteXOrx(business->accountno,pReturn1);

	}else{
		return ERROR;
	}

	/*业务时间*/
	pReturn1 = strtok_r(NULL, ATM_SEPARATOR_STRING, &pLeftStr);
	if (pReturn1 != NULL)
	{
		DeleteXOrx(business->businessdate,pReturn1);
		DeleteXOrx(fn->DateTime,pReturn1);
	}else{
		return ERROR;
	}

	/*业务金额*/
	pReturn1 = strtok_r(NULL, ATM_SEPARATOR_STRING, &pLeftStr);
	if (pReturn1 != NULL)
	{
		DeleteXOrx(business->moneytotal,pReturn1);
	}else{
		return ERROR;
	}
	/*业务金额*/
	pReturn1 = strtok_r(NULL, ATM_SEPARATOR_STRING, &pLeftStr);
	if (pReturn1 != NULL)
	{
		business->inorout=pReturn1[0];
		fn->BusinessType[0] = pReturn1[0];
	}else{
		return ERROR;
	}
	return SUCESS;
}
int ReadFSNOrBKFileName(const char *szFileName, FILENAME *pfilename)
{
	char strFileName[FILE_PATH_CHARNUM] = { 0 };
	char *pLeftStr = NULL;
	char *pReturn1;

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

	/*获取捆封签*/
	pReturn1 = strtok_r(NULL, SEPARATOR_STRING, &pLeftStr);
	if (pReturn1 != NULL) {
		    char *ending = strrchr(pReturn1, '.');
		    if(ending!=0){
		    	*ending = 0x00;
		    }
			memcpy(pfilename->PackageId, pReturn1, strlen(pReturn1));
			char c = *(pReturn1+9);
			if(isalpha(c)){
				sprintf(pfilename->Date, "20");
				memcpy(pfilename->Date + 2, pReturn1 + 11, DATE_LEN - 2);
				sprintf(pfilename->DateTime, "20");
				memcpy(pfilename->DateTime + 2, pReturn1 + 11, 14);
			}else{
				if (strstr(pReturn1, TIMEFILL_STRING) != 0) {/*采集软件获取日期*/
					memcpy(pfilename->Date, pReturn1 + 7, DATE_LEN);
					memcpy(pfilename->DateTime, pReturn1 + 7, 14);
				}else{
					memcpy(pfilename->Date, pReturn1, DATE_LEN);
					memcpy(pfilename->DateTime, pReturn1, 14);
				}
			}
	}
	pReturn1 = strtok_r(NULL, SEPARATOR_STRING, &pLeftStr);
	if (pReturn1 != NULL) {
		memcpy(pfilename->CounterNo, pReturn1, DESKNO_LEN);
	} else {
		memcpy(pfilename->CounterNo, "10", DESKNO_LEN);
	}
	return SUCESS;
}

//读取冠字号记录数
int ReadSnoNum(char szReadBuf[FILE_HEAD_SIZE])
{
		if (strcmp(szReadBuf, "") == 0)					//判断buf是否为空
		{
				return -1;
		}

		char strNumSNo[MAX_STRING_SIZE] = { 0 };
		memset(strNumSNo, 0, sizeof(strNumSNo));
		//冠字号记录条数
		sprintf(strNumSNo, "0x%02x%02x%02x%02x", (unsigned char) (szReadBuf[23]),
						(unsigned char) (szReadBuf[22]), (unsigned char) (szReadBuf[21]),
						(unsigned char) (szReadBuf[20]));

		int iNumSNoFSN = htoi(strNumSNo);

		return iNumSNoFSN;
}
