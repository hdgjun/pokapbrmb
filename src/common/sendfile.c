#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "db/db.h"
#include "db/dbroute.h"
#include "db/dbbankinfo.h"
#include "db/dbrouterule.h"
#include "db/dbbusinesslistatm.h"
#include "pokafile.h"
#include "pokalog.h"
#include "switch.h"
#include "common.h"
#include "pokapbrmb.h"

#define PASSIVE "on"
#define ACTIIVE "off"

static int CheckRule(ROUTERULE *rule, const int ruleSize, const char *fileType);

#if 0
static int Upload(const char *dir, const ROUTE *route, int batch);
#endif

static int Upload(const char *dir, const ROUTE *route, char *file,int batch);

static int StroeFiles(const char *dir, const ROUTE *route);
static char *GetFtpModel(ROUTE *route);

static char *GetFtpModel(ROUTE *route)
{
	if(route->model == 0){
		return ACTIIVE;
	}else{
		return PASSIVE;
	}
}

int UploadFile(ROUTE *route,ROUTERULE *rule,int ruleSize)
{
	DIR* p;
	struct dirent* dirlist;
	struct stat filestat;
	char szFolderPath[FILE_PATH_CHARNUM] = { 0 };
	char indir[FILE_PATH_CHARNUM] = { 0 };
	char temTarget[FILE_PATH_CHARNUM] = { 0 };
	char CmdStr[FILE_PATH_CHARNUM] = { 0 };
	char tempDir[FILE_PATH_CHARNUM] = { 0 };
	char zpath[FILE_PATH_CHARNUM] = { 0 };
	char zname[FILE_PATH_CHARNUM] = { 0 };

	sprintf(szFolderPath, "%s", route->localdir);
	p = opendir(szFolderPath);
	if (p == NULL) {
		printf("SwitchFile_Thread Open dir %s fail:%s\n", szFolderPath,
				strerror(errno));
		vLog("SwitchFile_Thread Open dir %s fail:%s", szFolderPath,
				strerror(errno));
		return ERROR;
	}
	int time = GetTimeInt();
	sprintf(tempDir, "%s/%u_%d", szFolderPath, (unsigned int) pthread_self(),
			time);
	JudgeSavePathExist(tempDir);

	char *fileType;

	vLog("starting upload dir:%s", route->localdir);
	int batch = 0;
	int iRet;
	while ((dirlist = readdir(p)) != NULL)
	{
		sprintf(indir, "%s/%s", szFolderPath, dirlist->d_name);
		stat(indir, &filestat);
		if (S_ISREG(filestat.st_mode)) {
			iRet = getInterval(filestat.st_mtime);
			if(iRet < 60)
			{
				vLog("file modify time[%d]",iRet);
				continue;
			}
			fileType = strrchr(dirlist->d_name, '.');
			if (CheckRule(rule, ruleSize, fileType) == ERROR)
			{
				continue;
			}

			if(CheckFileType(fileType) == ZIP_FILE_TYPE)
			{
				sprintf(CmdStr, "mv %s %s/%s.tmp", indir, tempDir, dirlist->d_name);
				my_system(CmdStr);
				sprintf(CmdStr, "mv %s/%s.tmp %s/%s", tempDir, dirlist->d_name, tempDir, dirlist->d_name);
				my_system(CmdStr);
			}else{
				memset(zpath,0x00,sizeof(zpath));
				memset(zname,0x00,sizeof(zname));
				sprintf(zname,"%s.zip.tmp",dirlist->d_name);
				sprintf(zpath,"%s.zip.tmp",indir);
				CompressFile(indir, zpath);
				if(CheckZipFIle(szFolderPath,zname, dirlist->d_name,filestat.st_size)==SUCESS)
				{//����zip�ɹ�
					DelFileOrDir(1, indir);
					sprintf(CmdStr, "mv %s.zip.tmp %s.zip", indir, indir);
					my_system(CmdStr);
				}else
				{//����zipʧ��
					DelFileOrDir(1, zname);
				}
				continue;
			}
			batch++;
			if (Upload(tempDir, route, dirlist->d_name,batch) == ERROR)
			{
				if (p != NULL)
				{
					closedir(p);
					p = NULL;
				}
				sprintf(temTarget, "%s/*", tempDir);
				Move(temTarget, szFolderPath);
				vLog("Upload error ,delete dir:%s", tempDir);
				DelFileOrDir(0, tempDir);
				return ERROR;
			} else
			{
				StroeFiles(tempDir, route);
			}

		}else
		{
			if((strstr(dirlist->d_name,"_")!=0)&&(strstr(tempDir,dirlist->d_name))==0)
			{
				vLog("error dir [%s] ",dirlist->d_name);
				sprintf(temTarget, "%s/%s/ftp*.log", szFolderPath,dirlist->d_name);
				DelFileOrDir(1, temTarget);
				sprintf(temTarget, "%s/%s/*.result", szFolderPath,dirlist->d_name);
				DelFileOrDir(1, temTarget);
				sprintf(temTarget, "%s/%s/*", szFolderPath,dirlist->d_name);
				Move(temTarget, szFolderPath);
				vLog("Upload error ,delete dir:%s", temTarget);
				sprintf(temTarget, "%s/%s/", szFolderPath,dirlist->d_name);
				DelFileOrDir(0, temTarget);
			}
		}

	}

	DelFileOrDir(0, tempDir);
	if (p != NULL)
	{
		closedir(p);
		p = NULL;
	}

	return SUCESS;
}

#if 0
int UploadFile(ROUTE *route,ROUTERULE *rule,int ruleSize)
{
	DIR* p;
	struct dirent* dirlist;
	struct stat filestat;
	char szFolderPath[FILE_PATH_CHARNUM] = { 0 };
	char indir[FILE_PATH_CHARNUM] = { 0 };
	char temTarget[FILE_PATH_CHARNUM] = { 0 };
	char CmdStr[FILE_PATH_CHARNUM] = { 0 };
	//char temName[FILE_PATH_CHARNUM] = { 0 };
	char tempDir[FILE_PATH_CHARNUM] = { 0 };

	sprintf(szFolderPath, "%s", route->localdir);
	p = opendir(szFolderPath);
	if (p == NULL) {
		printf("SwitchFile_Thread Open dir %s fail:%s\n", szFolderPath,
				strerror(errno));
		vLog("SwitchFile_Thread Open dir %s fail:%s", szFolderPath,
				strerror(errno));
		return ERROR;
	}
	int time = GetTimeInt();
	sprintf(tempDir, "%s/%u_%d", szFolderPath, (unsigned int) pthread_self(),
			time);
	JudgeSavePathExist(tempDir);

	char *fileType;
	//int i, iRet;
	int count = 0;
	int batch = 0;
	vLog("starting upload dir:%s", route->localdir);
	while ((dirlist = readdir(p)) != NULL) {
		sprintf(indir, "%s/%s", szFolderPath, dirlist->d_name);
		stat(indir, &filestat);
		if (S_ISREG(filestat.st_mode)) {
			fileType = strrchr(dirlist->d_name, '.');
			if (CheckRule(rule, ruleSize, fileType) == ERROR)
			{
				continue;
			}if(CheckFileType(fileType)==START_FILE_TYPE)
			{
				char oldname[MAX_STRING_SIZE] = {0};
				char newname[MAX_STRING_SIZE] = {0};
				char temname[MAX_STRING_SIZE] = {0};
				memcpy(temname,dirlist->d_name,strlen(dirlist->d_name) - strlen(fileType));
				sprintf(newname, "%s/%s", szFolderPath, temname);
				sprintf(oldname, "%s/%s", szFolderPath, dirlist->d_name);
				rename(oldname, newname);
				continue;
			}
			if(strstr(dirlist->d_name,"PACK_")!= 0)
			{
				sprintf(CmdStr, "mv %s/%s %s/%s.%s", szFolderPath,dirlist->d_name,
				szFolderPath,dirlist->d_name,START_FILE_STRING);
				my_system(CmdStr);
				DataType df;
				memset(&df,0x00,sizeof(df));
				memcpy(df.fileName,dirlist->d_name,strlen(dirlist->d_name));
				memcpy(df.filePath,szFolderPath,strlen(szFolderPath));
				unZip(&df);
				continue;
			}
			sprintf(CmdStr, "mv %s %s/%s", indir, tempDir, dirlist->d_name);
			my_system(CmdStr);
			count++;

			if (count == 1)
			{
				count = 0;
				/*����ѹ���ϴ�*/
				if (Upload(tempDir, route, batch) == ERROR)
				{
					if (p != NULL)
					{
						closedir(p);
						p = NULL;
					}
					sprintf(temTarget, "%s/*", tempDir);
					Move(temTarget, szFolderPath);
					vLog("Upload error ,delete dir:%s", tempDir);
					DelFileOrDir(0, tempDir);
					return ERROR;
				} else
				{
					batch =(++batch)%1000;
					StroeFiles(tempDir, route);
				}
			}
		}else
		{
			if((strstr(dirlist->d_name,"_")!=0)&&(strstr(tempDir,dirlist->d_name))==0)
			{
				vLog("error dir [%s] ",dirlist->d_name);
				sprintf(temTarget, "%s/%s/PACK_*", szFolderPath,dirlist->d_name);
				DelFileOrDir(1, temTarget);
				sprintf(temTarget, "%s/%s/*.log", szFolderPath,dirlist->d_name);
				DelFileOrDir(1, temTarget);
				sprintf(temTarget, "%s/%s/*.result", szFolderPath,dirlist->d_name);
				DelFileOrDir(1, temTarget);
				sprintf(temTarget, "%s/%s/*", szFolderPath,dirlist->d_name);
				Move(temTarget, szFolderPath);
				vLog("Upload error ,delete dir:%s", temTarget);
				sprintf(temTarget, "%s/%s/", szFolderPath,dirlist->d_name);
				DelFileOrDir(0, temTarget);
			}
		}

	}
	if (count != 0)
	{
		if (Upload(tempDir, route, batch) == ERROR)
		{
			if (p != NULL)
			{
				closedir(p);
				p = NULL;
			}
			sprintf(temTarget, "%s/*", tempDir);
			Move(temTarget, szFolderPath);
			DelFileOrDir(0, tempDir);
			return ERROR;
		} else
		{
			StroeFiles(tempDir, route);
		}
	}

	DelFileOrDir(0, tempDir);
	if (p != NULL)
	{
		closedir(p);
		p = NULL;
	}

	return SUCESS;
}
#endif

void RecoverDir(char *dir)
{
	DIR* p;
	struct dirent* dirlist;
	struct stat filestat;
	char szFolderPath[FILE_PATH_CHARNUM] = { 0 };
	char indir[FILE_PATH_CHARNUM] = { 0 };
	char temTarget[FILE_PATH_CHARNUM] = { 0 };
	char CmdStr[FILE_PATH_CHARNUM] = { 0 };
	//char temName[FILE_PATH_CHARNUM] = { 0 };
	char tempDir[FILE_PATH_CHARNUM] = { 0 };

	sprintf(szFolderPath, "%s", dir);
	p = opendir(szFolderPath);
	while ((dirlist = readdir(p)) != NULL) {
			sprintf(indir, "%s/%s", szFolderPath, dirlist->d_name);
			stat(indir, &filestat);
			if (!S_ISREG(filestat.st_mode))
			{
				if(strstr(dirlist->d_name,"_")!=0){
					sprintf(temTarget, "%s/%s/*", szFolderPath,dirlist->d_name);
					Move(temTarget, szFolderPath);
					sprintf(temTarget, "%s/%s/", szFolderPath,dirlist->d_name);
					vLog("Upload error ,delete dir:%s", temTarget);
					DelFileOrDir(0, temTarget);
				}
			}
	}
	if (p != NULL) {
		closedir(p);
		p = NULL;
	}
	return;
}
int DownFile(ROUTE *route,ROUTERULE *rule,int ruleSize)
{
	char szFolderPath[FILE_PATH_CHARNUM] = { 0 };
	char redir[FILE_PATH_CHARNUM] = { 0 };
	char temTarget[FILE_PATH_CHARNUM] = { 0 };
	char CmdStr[FILE_PATH_CHARNUM] = { 0 };
	char tempDir[FILE_PATH_CHARNUM] = { 0 };
	char getfilelist[FILE_PATH_CHARNUM] = { 0 };
	char apppath[FILE_PATH_CHARNUM] = { 0 };
	char result[FILE_PATH_CHARNUM] = { 0 };
    char shname[FILE_PATH_CHARNUM] = { 0 };
    char shgetfilename[FILE_PATH_CHARNUM] = { 0 };
    char shdeletename[FILE_PATH_CHARNUM] = { 0 };
	/*�ָ�Ŀ¼״̬*/
	RecoverDir(route->localdir);

	int time = GetTimeInt();
	sprintf(tempDir, "%s/%u_%d", route->localdir, (unsigned int) pthread_self(),
			time);
	JudgeSavePathExist(tempDir);
#if 0
	ROUTERULE rule[MAX_RULE];
	int ruleSize;
	memset(&rule, 0x00, sizeof(ROUTERULE) * MAX_RULE);
	DbRouteRule(&rule, &ruleSize, route->id);
#endif
	if (ruleSize == 0)
	{
		ruleSize = 1;
		//if(route->type == SFTP_DOWNLOAD)
		//{
			rule->fileextend[0]='1';
		//}
	}

	switch(route->type)
	{
		case FTP_DOWNLOAD:
			sprintf(shname,"%s",FTP_GETMESSGETSHELLNAME_STRING);
			sprintf(shgetfilename,"%s",FTP_GETSHELLNAME_STRING);
			sprintf(shdeletename,"%s",FTP_DELETEFILESHELLNAME_STRING);
			break;
		case SFTP_DOWNLOAD:
			sprintf(shname,"%s",SFTP_GETDIRSHELLNAME_STRING);
			break;
		default:
		{
			vLog("route[%d] type[%d] not in (ftp(0|1) | sftp(3|4)) error",route->id,route->type);
			return ERROR;
		}
	}
	GetProgramPath(apppath,POKA_HOME,DEF_INSTALL_PATH);

	switch (route->servicecode)
	{
		case SERVER_CODE_2:
			//sprintf(redir, "%s/%d/", route->remotedir, GetDateInterval(-1));
			//�ȷ����л�ȡatm crs�ļ�ר��
			//return GetLFATM(rule,ruleSize,route,tempDir);
		case SERVER_CODE_3:
		default:
			sprintf(redir, "%s/", route->remotedir);
	}

	int i;
	FILE *stream;
	for (i = 0; i < ruleSize; i++)
	{
		vLog("getting file from servicecode:%d[%d],%s  %s",route->servicecode,GetDateInterval(-1),redir,tempDir);
		sprintf(getfilelist, "%s/%u_%d_%d.list", tempDir, (unsigned int) pthread_self(),
				GetTimeInt(), i);
		sprintf(CmdStr,"sh %s/%s/%s %s %s %s %s %s %s %s %s %s"
				,apppath,SHELL_DIR,shname,route->ipaddr
				,route->port,tempDir,redir,route->user,route->password
				,getfilelist,(rule+i)->fileextend,GetFtpModel(route));
		my_system(CmdStr);

		stream = fopen(getfilelist,ONLYREAD_ACCESS_STRING);
		if(stream == NULL)
		{
			vLog("Open file[%s] error[%s]",getfilelist,strerror(errno));
			continue;
		}

		char reFile[FILE_PATH_CHARNUM] = {0};
		while(1)
		{
			memset(reFile,0x00,FILE_PATH_CHARNUM);
			if(NULL == fgets(reFile,FILE_PATH_CHARNUM,stream))
			{
				break;
			}
			if(strlen(reFile)<=0)
			{
				break;
			}

			strtrim(reFile);

			sprintf(CmdStr,"sh %s/%s/%s %s %s %s %s %s %s 1 %s %s"
			,apppath,SHELL_DIR,shgetfilename,route->ipaddr
			,route->port,tempDir,redir,route->user,route->password
			,reFile,GetFtpModel(route));
			my_system(CmdStr);

			sprintf(CmdStr,"%s/%s",tempDir,reFile);
			if(access(CmdStr,0) == 0)
			{//�����ļ��ɹ���׼��ɾ���������ϱ����ļ�
				vLog("get file[%s] sucess",reFile);
				sprintf(CmdStr,"sh %s/%s/%s %s %s %s %s %s 1 %s %s"
				,apppath,SHELL_DIR,shdeletename,route->ipaddr
				,route->port,redir,route->user,route->password
				,reFile,GetFtpModel(route));
				my_system(CmdStr);
			}else{
				vLog("get file[%s] error[%s]",CmdStr,strerror(errno));
			}
		}
		fclose(stream);
		DelFileOrDir(1, getfilelist);
		sprintf(temTarget, "%s/*", tempDir);
		Move(temTarget, route->localdir);

	}
	DelFileOrDir(0, tempDir);

	return SUCESS;
}
static int Upload(const char *dir, const ROUTE *route, char *file,int batch)
{
	char zFile[FILE_PATH_CHARNUM] = { 0 };
	char zpath[FILE_PATH_CHARNUM] = { 0 };
	char files[FILE_PATH_CHARNUM] = { 0 };
	char cmd[FILE_PATH_CHARNUM * 10] = { 0 };
	char apppath[FILE_PATH_CHARNUM] = { 0 };
	char result[FILE_PATH_CHARNUM] = { 0 };
	char shname[FILE_PATH_CHARNUM] = { 0 };
	FILE *rp = NULL;

	GetProgramPath(apppath,POKA_HOME,DEF_INSTALL_PATH);

	int time = GetTimeInt();

	sprintf(zFile, "%s", file);

	sprintf(result, "%s/%u_%d_%d.result", dir, (unsigned int) pthread_self(),time,batch);

	switch(route->type)
	{
		case FTP_UPLOAD:
			sprintf(shname,"%s",FTP_PUTSHELLNAME_STRING);
			break;
		case SFTP_UPLOAD:
			sprintf(shname,"%s",SFTP_PUTSHELLNAME_STRING);
			break;
		default:
		{
			vLog("route[%d] type[%d] not in (ftp(0|1) | sftp(3|4)) error",route->id,route->type);
			return ERROR;
		}
	}

	sprintf(cmd, "sh %s/%s/%s %s %s %s %s %s %s %s %s %s ", apppath, SHELL_DIR,
			shname, route->ipaddr, route->port, dir, route->remotedir,
			route->user, route->password, zFile, result,GetFtpModel(route));
	vLog("system result:%d",my_system(cmd));
	vLog("uploag cmd:%s", cmd);


	rp = fopen(result, ONLYREAD_ACCESS_STRING);
	if (NULL == rp) {
		DelFileOrDir(1, zpath);
		vLog("Open resule file:%s error");
		return ERROR;
	}

	char readbuf[FILE_PATH_CHARNUM] = { 0 };
	unsigned int ireadsize = 0;
	ireadsize = fread(readbuf, sizeof(char), FILE_PATH_CHARNUM, rp);

	fclose(rp);
	rp = NULL;
	vLog("result %s :%s", result, readbuf);
	int iRet;
	if (strstr(readbuf, FTP_SUCESS) != 0) {
		iRet = SUCESS;
	} else {
		iRet = ERROR;
	}
	DelFileOrDir(1, result);
	DelFileOrDir(1, zpath);
	return iRet;
}
#if 0
static int Upload(const char *dir, const ROUTE *route, int batch) {
	char zFile[FILE_PATH_CHARNUM] = { 0 };
	char zpath[FILE_PATH_CHARNUM] = { 0 };
	char files[FILE_PATH_CHARNUM] = { 0 };
	char cmd[FILE_PATH_CHARNUM * 10] = { 0 };
	char apppath[FILE_PATH_CHARNUM] = { 0 };
	char result[FILE_PATH_CHARNUM] = { 0 };
	char shname[FILE_PATH_CHARNUM] = { 0 };
	FILE *rp = NULL;

	GetProgramPath(apppath,POKA_HOME,DEF_INSTALL_PATH);

	int time = GetTimeInt();

	sprintf(zFile, "PACK_%u_%d_%d.zip", (unsigned int) pthread_self(), time, batch);
	sprintf(zpath, "%s/%s", dir, zFile);
	sprintf(files, "%s/*", dir);
	vLog("CompressFile[%s][%s]:%d",files,zpath,CompressFile(files, zpath));
	sprintf(result, "%s/%u_%d_%d.result", dir, (unsigned int) pthread_self(),
			time, batch);
    sleep(1);


	switch(route->type)
	{
		case FTP_UPLOAD:
			sprintf(shname,"%s",FTP_PUTSHELLNAME_STRING);
			break;
		case SFTP_UPLOAD:
			sprintf(shname,"%s",SFTP_PUTSHELLNAME_STRING);
			break;
		default:
		{
			vLog("route[%d] type[%d] not in (ftp(0|1) | sftp(3|4)) error",route->id,route->type);
			return ERROR;
		}
	}
	
	sprintf(cmd, "sh %s/%s/%s %s %s %s %s %s %s %s %s %s ", apppath, SHELL_DIR,
			shname, route->ipaddr, route->port, dir, route->remotedir,
			route->user, route->password, zFile, result,GetFtpModel(route));
	vLog("system result:%d",my_system(cmd));
	vLog("uploag cmd:%s", cmd);


	rp = fopen(result, ONLYREAD_ACCESS_STRING);
	if (NULL == rp) {
		DelFileOrDir(1, zpath);
		vLog("Open resule file:%s error");
		return ERROR;
	}

	char readbuf[FILE_PATH_CHARNUM] = { 0 };
	unsigned int ireadsize = 0;
	ireadsize = fread(readbuf, sizeof(char), FILE_PATH_CHARNUM, rp);

	fclose(rp);
	rp = NULL;
	vLog("result %s :%s", result, readbuf);
	int iRet;
	if (strstr(readbuf, FTP_SUCESS) != 0) {
		iRet = SUCESS;
	} else {
		iRet = ERROR;
	}
	DelFileOrDir(1, result);
	DelFileOrDir(1, zpath);
	return iRet;
}
#endif

static int StroeFiles(const char *dir, const ROUTE *route) {
	DIR* p;
	struct dirent* dirlist;
	struct stat filestat;
	char indir[FILE_PATH_CHARNUM] = { 0 };
	char desName[FILE_PATH_CHARNUM] = { 0 };
	char desDir[FILE_PATH_CHARNUM] = { 0 };
	p = opendir(dir);
	if (NULL == p) {
		vLog("Open dir:%s error", dir);
		return ERROR;
	}

	int date = GetDateInt();
	while ((dirlist = readdir(p)) != NULL) {
		sprintf(indir, "%s/%s", dir, dirlist->d_name);
		stat(indir, &filestat);
		if (S_ISREG(filestat.st_mode)) {
			sprintf(desName, "%s/%s/send/%s/%d/", g_param.FileStoreBasePath,
					g_param.handleDir, route->forwardbank, date);
			JudgeSavePathExist(desName);
			sprintf(desDir, "%s/%s", desName, dirlist->d_name);
			Move(indir, desDir);
		}
	}
	if(p){
		closedir(p);
		p = NULL;
	}
	return SUCESS;
}
static int CheckRule(ROUTERULE *rule, const int ruleSize, const char *fileType) {
	int i, iRet;
	if(fileType == NULL)return ERROR;
	if (ruleSize!=0)
	{
		for (i = 0; i < ruleSize; i++)
		{
			if (strstr(fileType, (rule + i)->fileextend) != 0)
			{
				return SUCESS;
			}
		}
		return ERROR;
	} else {
		if (strstr(fileType, TMP_FILE_STRING) != 0) {
			return ERROR;
		}
	}
	return SUCESS;
}


