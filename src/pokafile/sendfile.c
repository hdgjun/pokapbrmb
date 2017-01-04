#include <stdio.h>
#include <stdlib.h>
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

static int CheckRule(ROUTERULE *rule, const int ruleSize, const char *fileType);
static int Upload(const char *dir, const ROUTE *route, int batch);
static int StroeFiles(const char *dir, const ROUTE *route);

int UploadFile(ROUTE *route) {
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

	ROUTERULE rule[MAX_RULE];
	int ruleSize;
	memset(&rule, 0x00, sizeof(ROUTERULE) * MAX_RULE);
	DbRouteRule(&rule, &ruleSize, route->id);

	char *fileType;
	//int i, iRet;
	int count = 0;
	int batch = 0;
	vLog("starting upload dir:", route->localdir);
	while ((dirlist = readdir(p)) != NULL) {
		sprintf(indir, "%s/%s", szFolderPath, dirlist->d_name);
		stat(indir, &filestat);
		if (S_ISREG(filestat.st_mode)) {
			fileType = strrchr(dirlist->d_name, '.');
			if (CheckRule(rule, ruleSize, fileType) == ERROR) {
				continue;
			}
			sprintf(CmdStr, "mv %s %s/%s", indir, tempDir, dirlist->d_name);
			system(CmdStr);
			count++;

			if (count == 20) {
				count = 0;
				/*分批压缩上传*/
				if (Upload(tempDir, route, batch) == ERROR) {
					if (p != NULL) {
						closedir(p);
						p = NULL;
					}
					sprintf(temTarget, "%s/*", tempDir);
					Move(temTarget, szFolderPath);
					vLog("Upload error ,delete dir:%s", tempDir);
					DelFileOrDir(0, tempDir);
					return ERROR;
				} else {
					batch++;
					StroeFiles(tempDir, route);
				}
			}
		}else{
			if((strstr(dirlist->d_name,"_")!=0)&&(strstr(tempDir,dirlist->d_name))==0){
				vLog("error dir [%s] ",dirlist->d_name);
				sprintf(temTarget, "%s/%s/PACK_*", szFolderPath,dirlist->d_name);
				DelFileOrDir(1, temTarget);
				sprintf(temTarget, "%s/%s/*", szFolderPath,dirlist->d_name);
				Move(temTarget, szFolderPath);
				vLog("Upload error ,delete dir:%s", temTarget);
				sprintf(temTarget, "%s/%s/", szFolderPath,dirlist->d_name);
				DelFileOrDir(0, temTarget);
			}
		}

	}
	if (count != 0) {
		if (Upload(tempDir, route, batch) == ERROR) {
			if (p != NULL) {
				closedir(p);
				p = NULL;
			}
			sprintf(temTarget, "%s/*", tempDir);
			Move(temTarget, szFolderPath);
			DelFileOrDir(0, tempDir);
			return ERROR;
		} else {
			StroeFiles(tempDir, route);
		}
	}

	DelFileOrDir(0, tempDir);
	if (p != NULL) {
		closedir(p);
		p = NULL;
	}

	return SUCESS;
}

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
int DownFile(ROUTE *route) {
	char szFolderPath[FILE_PATH_CHARNUM] = { 0 };
	char redir[FILE_PATH_CHARNUM] = { 0 };
	char temTarget[FILE_PATH_CHARNUM] = { 0 };
	char CmdStr[FILE_PATH_CHARNUM] = { 0 };
	char tempDir[FILE_PATH_CHARNUM] = { 0 };
	char getfilelist[FILE_PATH_CHARNUM] = { 0 };
	char apppath[FILE_PATH_CHARNUM] = { 0 };
	char result[FILE_PATH_CHARNUM] = { 0 };

	/*恢复目录状态*/
	RecoverDir(route->localdir);

	int time = GetTimeInt();
	sprintf(tempDir, "%s/%u_%d", route->localdir, (unsigned int) pthread_self(),
			time);
	JudgeSavePathExist(tempDir);

	ROUTERULE rule[MAX_RULE];
	int ruleSize;
	memset(&rule, 0x00, sizeof(ROUTERULE) * MAX_RULE);
	DbRouteRule(&rule, &ruleSize, route->id);

	switch (route->servicecode) {
	case SERVER_CODE_2:
		sprintf(redir, "%s/%d/", route->remotedir, GetDateInterval(-1));
		break;
	case SERVER_CODE_3:
	default:
		sprintf(redir, "%s/", route->remotedir);
	}

	if (ruleSize == 0) {
		ruleSize = 1;
	}

	GetProgramPath(apppath,POKA_HOME,DEF_INSTALL_PATH);
	int i;

	for (i = 0; i < ruleSize; i++) {
		vLog("getting file from servicecode:%d[%d],%s  %s",route->servicecode,GetDateInterval(-1),redir,tempDir);
		sprintf(getfilelist, "%s/%u_%d_%d.list", tempDir, (unsigned int) pthread_self(),
				GetTimeInt(), i);
		sprintf(CmdStr,"sh %s/%s/%s %s %s %s %s %s %s %s %s"
				,apppath,SHELL_DIR,FTP_GETDIRSHELLNAME_STRING,route->ipaddr
				,route->port,tempDir,redir,route->user,route->password
				,getfilelist,rule[i].fileextend);
		system(CmdStr);
		sprintf(temTarget, "%s/*", tempDir);
		Move(temTarget, route->localdir);
		DelFileOrDir(1, getfilelist);
	}
	DelFileOrDir(0, tempDir);

	return SUCESS;
}

static int Upload(const char *dir, const ROUTE *route, int batch) {
	char zFile[FILE_PATH_CHARNUM] = { 0 };
	char zpath[FILE_PATH_CHARNUM] = { 0 };
	char files[FILE_PATH_CHARNUM] = { 0 };
	char cmd[FILE_PATH_CHARNUM * 10] = { 0 };
	char apppath[FILE_PATH_CHARNUM] = { 0 };
	char result[FILE_PATH_CHARNUM] = { 0 };
	FILE *rp = NULL;

	GetProgramPath(apppath,POKA_HOME,DEF_INSTALL_PATH);

	int time = GetTimeInt();

	sprintf(zFile, "PACK_%u_%d_%d.zip", (unsigned int) pthread_self(), time, batch);
	sprintf(zpath, "%s/%s", dir, zFile);
	sprintf(files, "%s/*", dir);
	CompressFile(files, zpath);
	sprintf(result, "%s/%u_%d_%d.result", dir, (unsigned int) pthread_self(),
			time, batch);

	sprintf(cmd, "sh %s/%s/%s %s %s %s %s %s %s %s %s ", apppath, SHELL_DIR,
	FTP_PUTSHELLNAME_STRING, route->ipaddr, route->port, dir, route->remotedir,
			route->user, route->password, zFile, result);
	system(cmd);
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
	return SUCESS;
}
static int CheckRule(ROUTERULE *rule, const int ruleSize, const char *fileType) {
	int i, iRet;
	if (ruleSize) {
		for (i = 0; i < ruleSize; i++) {
			if (strstr(fileType, (rule + i)->fileextend) != 0) {
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

int Start_service(ROUTE *route) {
	route->status = S_START;
	return DbRoute(DBS_UPDATE, route);
}
int Stop_service(ROUTE *route) {
	route->status = S_STOP;
	return DbRoute(DBS_UPDATE, route);
}

