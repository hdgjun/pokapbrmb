#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "pokacfg.h"
#include "filepath.h"
#include "switch.h"
#include "pokapbrmb.h"

void FillParamStruct(char szKey[PARAM_NUM][MAX_STRING_SIZE],
		char szValue[PARAM_NUM][MAX_STRING_SIZE], Param *param);
int JudgeParamExist(char *pParamName, char *pParamString, char *pParamValue,
		FILE *pConfFile, char *pNoteText);
void JudgeParam(Param *param);
void UseDefaultValue(Param *param);
//配置文件读取参数
int ReadParamFromIniFile(Param *param, char *app_home) {
	char szKey[PARAM_NUM][MAX_STRING_SIZE];
	char szValue[PARAM_NUM][MAX_STRING_SIZE];

	memset(szKey, 0x00, PARAM_NUM * MAX_STRING_SIZE);
	memset(szValue, 0x00, PARAM_NUM * MAX_STRING_SIZE);
	//从配置文件中读取
	char ConfFileBuf[CONFFILE_BUF_SIZE] = { 0 };
	FILE *pConfFile = NULL;

	char ConfFilePath[FILE_PATH_CHARNUM] = { 0 };
	char apppath[FILE_PATH_CHARNUM] = { 0 };


	sprintf(ConfFilePath, "%s/%s/%s", GetProgramPath(apppath,APP_NAME,DEF_INSTALL_PATH),CFG_DIR, CONFILE_NAME);

	pConfFile = fopen(ConfFilePath, ONLYREAD_ACCESS_STRING);
	if (pConfFile == NULL) {
		printf("open conf file fail:%s\n", strerror(errno));
		return -1;
	}

	int i = 0;
	while (fgets(ConfFileBuf, CONFFILE_BUF_SIZE - 1, pConfFile)) //每行字符不超过199才正常获取
	{

		if (strlen(ConfFileBuf) <= 1) //注意这儿是1不是0，因为尽管是空行，它也会读入换行符，strlen(s)=1
				{
			continue;
		}

		if (NULL == strtrim(ConfFileBuf)) {
			printf("Trim string fail\n");
			continue;
		}

		//printf("---------->ConfFileBuf:%s\n",ConfFileBuf);
		int len = 0;
		len = strlen(ConfFileBuf);
		if (len <= 0) {
			continue;
		} else {
			char *p2 = strchr(ConfFileBuf, '=');
			if (p2 == NULL) {
				continue;
			}
			*p2++ = '\0';
			strcpy(szKey[i], ConfFileBuf);
			strcpy(szValue[i], p2);
			i++;
		}
	}
	fclose(pConfFile);
	pConfFile = NULL;
	FillParamStruct(szKey, szValue, param); //填充参数结构体

	return 0;
}

//填充参数结构体
void FillParamStruct(char szKey[PARAM_NUM][MAX_STRING_SIZE],
		char szValue[PARAM_NUM][MAX_STRING_SIZE], Param *param) {
	int i;
	param->SaveImage = -1;


	for (i = 0; i <= PARAM_NUM; i++) {

		if (strcmp(szKey[i], DBUSER_KEY) == 0) {
			strcpy(param->DBUser, szValue[i]);
			continue;
		}
		if (strcmp(szKey[i], DBPASSWORD_KEY) == 0) {
			strcpy(param->DBPassword, szValue[i]);
			continue;
		}
		if (strcmp(szKey[i], DBSERVICENAME_KEY) == 0) {
			strcpy(param->DBServiceName, szValue[i]);
			continue;
		}
		if (strcmp(szKey[i], DBIP_KEY) == 0) {
			strcpy(param->DBIP, szValue[i]);
			continue;
		}
		if (strcmp(szKey[i], DBPORT_KEY) == 0) {
			strcpy(param->DBPort, szValue[i]);
			continue;
		}
		if (strcmp(szKey[i], FILESTROEBASEPATH_KEY) == 0) {
			strcpy(param->FileStoreBasePath, szValue[i]);
			continue;
		}
		if (strcmp(szKey[i], SWITCHDIR_KEY) == 0) {
			strcpy(param->SwitchDir, szValue[i]);
			continue;
		}
		if (strcmp(szKey[i], INSERTDIR_KEY) == 0) {
			strcpy(param->InsertDir, szValue[i]);
			continue;
		}
		if (strcmp(szKey[i], SENDFILEDIR_KEY) == 0) {
			strcpy(param->SendFileDIr, szValue[i]);
			continue;
		}
		if (strcmp(szKey[i], UPDATEDIR_KEY) == 0) {
			strcpy(param->UpdateDIr, szValue[i]);
			continue;
		}
		if (strcmp(szKey[i], ERRORDIR_KEY) == 0) {
			strcpy(param->ErrorDir, szValue[i]);
			continue;
		}
		if (strcmp(szKey[i], HANDLEDIR_KEY) == 0) {
			strcpy(param->handleDir, szValue[i]);
			continue;
		}

		if (strcmp(szKey[i], THREADSIZE_KEY) == 0) {
			param->ThreadSize = atoi(szValue[i]);
			continue;
		}
		if (strcmp(szKey[i], SLEEPTIME_KEY) == 0) {
			param->SleepTime = atoi(szValue[i]);
			continue;
		}

		if (strcmp(szKey[i], BANKNO_KEY) == 0) {
			strcpy(param->BankNo, szValue[i]);
			continue;
		}
		if (strcmp(szKey[i], PBNO_KEY) == 0) {
				strcpy(param->PBno, szValue[i]);
				continue;
		}

		if (strcmp(szKey[i], IMAGE_PATH_KEY) == 0) {
			strcpy(param->ImagePath, szValue[i]);
			continue;
		}

		if(strcmp(szKey[i],IMAGE_DIR_KEY) == 0)
		{
			strcpy(param->ImageDir,szValue[i]);
			continue;
		}

		if(strcmp(szKey[i],IMAGE_SAVE_KEY) == 0)
		{
			param->SaveImage=atoi(szValue[i]);
			continue;
		}

		if(strcmp(szKey[i],LOG_SAVE_DAY_KEY) == 0)
		{
			param->LogSaveDay=atoi(szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],FILE_SAVE_DAY_KEY) == 0)
		{
			param->FileSaveDay = atoi(szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],IMAGE_SAVE_DAY_KEY) == 0)
		{
			param->ImageSaveDay = atoi(szValue[i]);
			continue;
		}
#if 0
		if(strcmp(szKey[i],GETORDERSPACE_STRING) == 0)
		{
			param->iGetOrderSpace = atoi(szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],DELMONSSPACE_STRING) == 0)
		{
			param->iDelMonsSpace = atoi(szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],THREADNUM_STRING) == 0)
		{
			param->iThreadNum= atoi(szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],LOCALNEEDSAVEIMAGE_STRING) == 0)
		{
			strcpy(param->LocalNeedSaveSnoImage,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],HAVESORTCENTER_STRING) == 0)
		{
			strcpy(param->HaveSortCenter,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],SORTCENTERNO_STRING) == 0)
		{
			strcpy(param->SortCenterNo,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],NEDCOMPAREDOUBTSNO_STRING) == 0)
		{
			strcpy(param->NeedCompareDoubtSno,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],NEEDDELDBDATA_STRING) == 0)
		{
			strcpy(param->NeedDelDbData,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],DELMONDATABYCOLTIME_STRING) == 0)
		{
			strcpy(param->DelMoneydataByColtime,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],SAVEFILEBYDAYS_STRING) == 0)
		{
			strcpy(param->SaveFileByDays,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],FILESAVEDAYSORMONS_STRING) == 0)
		{
			param->iFileSaveDaysOrMons = atoi(szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],SAVEDATAATONETABLE_STRING) == 0)
		{
			strcpy(param->SaveSnoDataAtOneTable,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],CBSAVEIMAGEFILEPATH_STRING) == 0)
		{
			strcpy(param->CBSaveImageFilePath,szValue[i]);
			//printf("--------->param->CBSaveImageFilePath:%s\n",param->CBSaveFsnFilePath);
			continue;
		}
		if(strcmp(szKey[i],CBSAVEFSNFILEPATH_STRING) == 0)
		{
			strcpy(param->CBSaveFsnFilePath,szValue[i]);
			//printf("--------->param->CBSaveFsnFilePath:%s\n",param->CBSaveFsnFilePath);
			continue;
		}
		if(strcmp(szKey[i],PBSAVEFSNFILEPATH_STRING) == 0)
		{
			strcpy(param->PBSaveFsnFilePath,szValue[i]);
			//printf("--------->param->PBSaveFsnFilePath:%s\n",param->PBSaveFsnFilePath);
			continue;
		}
		if(strcmp(szKey[i],BANKNO_STRING) == 0)
		{
			strcpy(param->BankNo,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],FTPSERVERIP_STRING) == 0)
		{
			strcpy(param->FTPServerIP,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],ONLYPUTSORTFILE_STRING) == 0)
		{
			strcpy(param->OnlyPutSortCenterFile,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],WEIBAOFSNFILEPATH1_STRING) == 0)
		{
			strcpy(param->WeibaoFsnFilePath1,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],WEIBAOFSNFILEPATH2_STRING) == 0)
		{
			strcpy(param->WeibaoFsnFilePath2,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],WEIBAOFSNFILEPATH3_STRING) == 0)
		{
			strcpy(param->WeibaoFsnFilePath3,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],CBFSNFILEPATH1_STRING) == 0)
		{
			strcpy(param->CBFsnFilePath1,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],CBFSNFILEPATH2_STRING) == 0)
		{
			strcpy(param->CBFsnFilePath2,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],CBFSNFILEPATH3_STRING) == 0)
		{
			strcpy(param->CBFsnFilePath3,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],PBFSNFILEPATH1_STRING) == 0)
		{
			strcpy(param->PBFsnFilePath1,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],PBFSNFILEPATH2_STRING) == 0)
		{
			strcpy(param->PBFsnFilePath2,szValue[i]);
			continue;
		} //PBPutCBFILEPATH_STRING
		if(strcmp(szKey[i],PBFSNFILEPATH3_STRING) == 0)
		{
			strcpy(param->PBFsnFilePath3,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],PBFSNFILEPATH4_STRING) == 0)
		{
			strcpy(param->PBFsnFilePath4,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],PBFSNFILEPATH5_STRING) == 0)
		{
			strcpy(param->PBFsnFilePath5,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],PBFSNFILEPATH6_STRING) == 0)
		{
			strcpy(param->PBFsnFilePath6,szValue[i]);
			continue;
		}
		/*if(strcmp(szKey[i],PBFSNFILEPATH7_STRING) == 0)
		 {
		 strcpy(param->PBFsnFilePath7,szValue[i]);
		 continue;
		 }*/
		if(strcmp(szKey[i],PBPUTCBFILEPATH_STRING) == 0)
		{
			strcpy(param->PBPutCBFilePath,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],ERRORFSNFILEPATH_STRING) == 0)
		{
			strcpy(param->ErrorFsnFilePath,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],FINALFSNFILEPATH_STRING) == 0)
		{
			strcpy(param->FinalFsnFilePath,szValue[i]);
			continue;
		}
		if(strcmp(szKey[i],FSNTEXTFILEPATH_STRING) == 0)
		{
			strcpy(param->FsnTextFilePath,szValue[i]);
			continue;
		}
#endif
	}
	UseDefaultValue(param);
	//JudgeParam(param);//判断参数
}
void UseDefaultValue(Param *param) {

	if (strlen(param->FileStoreBasePath) <= 0) {
		strcpy(param->FileStoreBasePath, DEF_FILE_STORE_BASE_PATH);
	}
	if (strlen(param->SwitchDir) <= 0) {
		strcpy(param->SwitchDir, DEF_SWITCH_DIR);
	}
	if (strlen(param->InsertDir) <= 0) {
		strcpy(param->InsertDir, DEF_INSERT_DIR);
	}
	if (strlen(param->SendFileDIr) <= 0) {
		strcpy(param->SendFileDIr, DEF_SEND_FILE_DIR);
	}
	if (strlen(param->ErrorDir) <= 0) {
		strcpy(param->ErrorDir, DEF_ERROR_DIR);
	}
	if (strlen(param->UpdateDIr) <= 0) {
		strcpy(param->UpdateDIr, DEF_UPDATE_DIR);
	}

	if (strlen(param->ImageDir) <= 0) {
		strcpy(param->ImageDir, DEF_IMAGE_DIR);
	}
	if (strlen(param->ImagePath) <= 0) {
		strcpy(param->ImagePath, param->FileStoreBasePath);
	}
	if (param->ThreadSize <= 0 || param->ThreadSize > MAX_THREAD_NUM) {
		param->ThreadSize = DEF_THREAD_SIZE;
	}
	if (param->SleepTime <= 0 || param->SleepTime > MAX_SLEEP_TIME) {
		param->SleepTime = DEF_SLEEP_TIME;
	}
	if (strlen(param->handleDir) <= 0) {
		strcpy(param->handleDir, DEF_HANDLE_DIR);
	}
	if (strlen(param->BankNo) <= 0) {
		strcpy(param->BankNo, DEF_BANKNO);
	}

	if (param->SaveImage <= 0) {
		param->SaveImage = DEF_IMAGE_SAVE;
	}
	if (param->LogSaveDay <= 0) {
		param->LogSaveDay = DEF_LOG_SAVE_DAY;
	}
	if (param->FileSaveDay <= 0) {
		param->FileSaveDay = DEF_FILE_SAVE_DAY;
	}
	if (param->ImageSaveDay <= 0) {
		param->ImageSaveDay = DEF_IMAGE_SAVE_DAY;
	}
}
//判断参数
void JudgeParam(Param *param) {
	//拼接配置文件路径
	FILE *pConfFile = NULL;
	char Path[FILE_PATH_CHARNUM] = { 0 };
	char ConfFilePath[FILE_PATH_CHARNUM] = { 0 };
	getcwd(Path, FILE_PATH_CHARNUM);

	sprintf(ConfFilePath, "%s/%s", Path, CONFILE_NAME);
	printf("Conf file dir is:%s\n", ConfFilePath);
	pConfFile = fopen(ConfFilePath, ADD_ACCESS_STRING); //追加方式打开
	if (NULL == pConfFile) {
		printf("FillParamStruct pConfFile is null\n");
		return;
	}

#if 0
	sprintf(NoteText,"%s","0:YES 1:NO");
	sprintf(ParamValue,"%s","1");	//默认按月保存文件
	//只有配置文件中无参数时赋初值
	if(-2 == JudgeParamExist(param->SaveFileByDays,SAVEFILEBYDAYS_STRING,ParamValue,pConfFile,NoteText))
	{
		sprintf(param->SaveFileByDays,"%s",ParamValue);	//给参数赋初值
	}

	sprintf(NoteText,"%s","fsn file or log file save days(s) or month(s)");
	sprintf(ParamValue,"%s","1");
	//printf("----->FillParamStruct param->iFileSaveDaysOrMons:%d\n",param->iFileSaveDaysOrMons);
	if(0 > param->iFileSaveDaysOrMons)
	{
		if(-2 == JudgeParamExist("",FILESAVEDAYSORMONS_STRING,ParamValue,pConfFile,NoteText))
		{
			param->iFileSaveDaysOrMons = 1;
		}
	}

	sprintf(NoteText,"%s","Fsn text file path");
	sprintf(ParamValue,"%s","/home/poka/fsn_text/");
	if(-2 == JudgeParamExist(param->FsnTextFilePath,FSNTEXTFILEPATH_STRING,ParamValue,pConfFile,NoteText))
	{
		sprintf(param->FsnTextFilePath,"%s",ParamValue);
	}

	sprintf(NoteText,"%s","0:By file 1:Batch insert 2:Old insert");	//默认文本导入数据库
	sprintf(ParamValue,"%s","0");
	if(-2 == JudgeParamExist(param->InsertSnoByFile,INSERTSNOBYFILE_STRING,ParamValue,pConfFile,NoteText))
	{
		sprintf(param->InsertSnoByFile,"%s",ParamValue);
	}

	sprintf(NoteText,"%s","0:YES 1:NO");	//是否只put清分中心数据，默认否
	sprintf(ParamValue,"%s","1");
	if(-2 == JudgeParamExist(param->OnlyPutSortCenterFile,ONLYPUTSORTFILE_STRING,ParamValue,pConfFile,NoteText))
	{
		sprintf(param->OnlyPutSortCenterFile,"%s",ParamValue);
	}

	sprintf(NoteText,"%s","Sort Center no");	//清分中心编号
	sprintf(ParamValue,"%s","0400");
	if(-2 == JudgeParamExist(param->SortCenterNo,SORTCENTERNO_STRING,ParamValue,pConfFile,NoteText))
	{
		sprintf(param->SortCenterNo,"%s",ParamValue);
	}

	sprintf(NoteText,"%s","CB hand in file path");
	sprintf(ParamValue,"%s","/home/poka/fsn_cb_handin/");
	if(-2 == JudgeParamExist(param->PBFsnFilePath4,PBFSNFILEPATH4_STRING,ParamValue,pConfFile,NoteText))
	{
		sprintf(param->PBFsnFilePath4,"%s",ParamValue);
	}

	sprintf(NoteText,"%s","SortCenter hand in file path");
	sprintf(ParamValue,"%s","/home/poka/fsn_sc_put/");
	if(-2 == JudgeParamExist(param->PBFsnFilePath5,PBFSNFILEPATH5_STRING,ParamValue,pConfFile,NoteText))
	{
		sprintf(param->PBFsnFilePath5,"%s",ParamValue);
	}

	sprintf(NoteText,"%s","SortCenter Inserted file path");
	sprintf(ParamValue,"%s","/home/poka/fsn_sc_ins/");
	if(-2 == JudgeParamExist(param->PBFsnFilePath6,PBFSNFILEPATH6_STRING,ParamValue,pConfFile,NoteText))
	{
		sprintf(param->PBFsnFilePath6,"%s",ParamValue);
	}

	/*sprintf(NoteText,"%s","SortCenter PB already put file path");
	 sprintf(ParamValue,"%s","/home/poka/fsn_sc_get/");
	 if(-2 == JudgeParamExist(param->PBFsnFilePath7,PBFSNFILEPATH7_STRING,ParamValue,pConfFile,NoteText))
	 {
	 sprintf(param->PBFsnFilePath7,"%s",ParamValue);
	 }*/

	sprintf(NoteText,"%s","thread num(num>=1,if HaveSortCenter=0 ThreadNum is double in fact!)");
	sprintf(ParamValue,"%s","10");			//默认10个线程(有现金中心共10*2个)
	if(0 > param->iThreadNum)
	{
		if(-2 == JudgeParamExist("",THREADNUM_STRING,ParamValue,pConfFile,NoteText))
		{
			param->iThreadNum = 10;
		}
	}

	sprintf(NoteText,"%s","0:YES 1:NO");
	sprintf(ParamValue,"%s","1");			//默认本地不保存图片
	if(-2 == JudgeParamExist(param->LocalNeedSaveSnoImage,LOCALNEEDSAVEIMAGE_STRING,ParamValue,pConfFile,NoteText))
	{
		sprintf(param->LocalNeedSaveSnoImage,"%s",ParamValue);
	}

	sprintf(NoteText,"%s","0:YES 1:NO");
	sprintf(ParamValue,"%s","1");			//默认无清分中心
	if(-2 == JudgeParamExist(param->HaveSortCenter,HAVESORTCENTER_STRING,ParamValue,pConfFile,NoteText))
	{
		sprintf(param->HaveSortCenter,"%s",ParamValue);
	}

	sprintf(NoteText,"%s","0:YES 1:NO");
	sprintf(ParamValue,"%s","1");			//默认不与数据库比对黑名单
	if(-2 == JudgeParamExist(param->NeedCompareDoubtSno,NEDCOMPAREDOUBTSNO_STRING,ParamValue,pConfFile,NoteText))
	{
		sprintf(param->NeedCompareDoubtSno,"%s",ParamValue);
	}

	sprintf(NoteText,"%s","0:YES 1:NO");
	sprintf(ParamValue,"%s","0");			//默认需在程序中删除旧数据库数据
	if(-2 == JudgeParamExist(param->NeedDelDbData,NEEDDELDBDATA_STRING,ParamValue,pConfFile,NoteText))
	{
		sprintf(param->NeedDelDbData,"%s",ParamValue);
	}

	sprintf(NoteText,"%s","0:Coltime 1:InsertDate");
	sprintf(ParamValue,"%s","0");			//默认按清点时间删除冠字号数据
	if(-2 == JudgeParamExist(param->DelMoneydataByColtime,DELMONDATABYCOLTIME_STRING,ParamValue,pConfFile,NoteText))
	{
		sprintf(param->DelMoneydataByColtime,"%s",ParamValue);
	}

	printf("---->FillParamStruct param->SaveFileByDays:%s\n",param->SaveFileByDays);
	printf("---->FillParamStruct param->iFileSaveDaysOrMons:%d\n",param->iFileSaveDaysOrMons);
	printf("---->FillParamStruct param->iThreadNum:%d\n",param->iThreadNum);
	printf("---->FillParamStruct param->LocalNeedSaveSnoImage:%s\n",param->LocalNeedSaveSnoImage);
	printf("---->FillParamStruct param->NeedCompareDoubtSno:%s\n",param->NeedCompareDoubtSno);
	printf("---->FillParamStruct param->NeedDelDbData:%s\n",param->NeedDelDbData);
	printf("---->FillParamStruct param->DelMoneydataByColtime:%s\n",param->DelMoneydataByColtime);

#endif
	if (NULL != pConfFile) {
		fclose(pConfFile);
		pConfFile = NULL;
	}

}

//判断参数是否存在，不存在则添加
int JudgeParamExist(char *pParamName, char *pParamString, char *pParamValue,
		FILE *pConfFile, char *pNoteText) {
	unsigned int uiwrite = 0;
	char AddText[20 + FILE_PATH_CHARNUM] = { 0 };

	if (NULL == pConfFile) {
		printf("JudgeParamExist pConfFile is null\n");
		return -1;
	}

	if (0 == strlen(pParamName)) {
		fseek(pConfFile, 0, SEEK_END);
		//拼接默认配置参数内容
		sprintf(AddText, "%s;%s: %s%s%s=%s%s", ENTER_CHAR_STRING, pParamString,
				pNoteText, ENTER_CHAR_STRING, pParamString, pParamValue,
				ENTER_CHAR_STRING);
		//;FsnTextFilePath: Fsn text file path
		printf("JudgeParamExist AddText:%s\n", AddText);
		uiwrite = fwrite(AddText, sizeof(char), strlen(AddText), pConfFile);
		//fprintf(pConfFile, "%s", AddText);
		if (0 >= uiwrite) {
			printf("Write param %s fail\n", pParamString);
		}

		return -2;
	}

	return 0;
}
