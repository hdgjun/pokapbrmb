#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "db/db.h"
#include "datecut.h"
#include "pokafile.h"
#include "pokalog.h"
#include "switch.h"
#include "db/dbroute.h"
#include "db/dbbankinfo.h"
#include "db/dbrouterule.h"
#include "db/dbbusinesslistatm.h"
#include "common.h"
#include "pokapbrmb.h"



#define TRY_CONNECT_DB  \
	int SLEEPTIME = 1;\
	while (1) {\
		if (ThreadConnectDB() == SUCESS) {\
			break;\
		}\
		sleep(10 * SLEEPTIME);\
		SLEEPTIME++;\
		SLEEPTIME = SLEEPTIME % 180;\
	}\
	SLEEPTIME = 1;\
	vLog("ThreadConnectDB:%d  ok");\

extern cir_queue_t gQue;
extern Param g_param;
extern pthread_attr_t attr;

static void *SendTask(void *sp);
static int GetBanknoAndAgencyno(const char *szFileName, FILENAME *pfilename);
static void printRout(ROUTE *route );

void *SwitchFileThread(void *pt)
{
	DIR* p;
	struct dirent* dirlist;
	struct stat filestat;
	char szFolderPath[FILE_PATH_CHARNUM] = { 0 };
	char indir[FILE_PATH_CHARNUM] = { 0 };
	char temTarget[FILE_PATH_CHARNUM] = { 0 };
	char CmdStr[FILE_PATH_CHARNUM] = { 0 };
	char temName[FILE_PATH_CHARNUM] = { 0 };
	pthread_t *th = (pthread_t *) pt;

	TRY_CONNECT_DB

	sprintf(szFolderPath, "%s/%s/", g_param.FileStoreBasePath,
			g_param.SwitchDir);

#ifdef   DEBUG
	vLog("dir:%s", szFolderPath);
#endif
	while(1)
	{
		p = opendir(szFolderPath);
		if (p == NULL)
		{
			printf("SwitchFile_Thread Open dir %s fail:%s\n", szFolderPath,
					strerror(errno));
			vLog("SwitchFile_Thread Open dir %s fail:%s", szFolderPath,
					strerror(errno));
			return (void *) ERROR;
		}
		char *fileType;
		DataType df;
		int fs = 0;
		int tp;
		FILENAME fn;
		ATMBUSINESSLIST business;
		while ((dirlist = readdir(p)) != NULL) {
			sprintf(indir, "%s/%s", szFolderPath, dirlist->d_name);
			stat(indir, &filestat);

			if (S_ISREG(filestat.st_mode)) {
				fileType = strrchr(dirlist->d_name, '.');
				if (((tp = CheckFileType(fileType)) == ERROR)
						|| tp == START_FILE_TYPE) {
					continue;
				}

				if(tp == ZIP_FILE_TYPE)
				{
					memset(&df, 0x00, sizeof(DataType));
					memcpy(df.filePath, szFolderPath, strlen(szFolderPath));
					memcpy(df.fileName, dirlist->d_name, strlen(dirlist->d_name));
					df.filesize = filestat.st_size;
					df.fileType = tp;
					if(StartFile(&df)==SUCESS){
						ZIPFile(&df);
					}
					continue;
				}

#ifdef PEOPLEBANK
				GetBanknoAndAgencyno(dirlist->d_name, &fn);

				if (CheckSortCenter(fn.BankNo)==SUCESS) {
#endif
				/*[商行]文件发送到转发目录(SendFileDIr=fsn_sc_ins)下的人行子目录 */
				/*[人行]清分中心文件分发到转发目录*/
#ifndef PEOPLEBANK
				sprintf(temTarget, "%s/%s/%s/", g_param.FileStoreBasePath,
						g_param.SendFileDIr, g_param.PBno);
#else
				sprintf(temTarget, "%s/%s/%s/", g_param.FileStoreBasePath,g_param.SendFileDIr,fn.AgencyNo);
#endif
				JudgeSavePathExist(temTarget);

				sprintf(CmdStr, "cp %s/%s %s/%s.%s", szFolderPath,
						dirlist->d_name, temTarget, dirlist->d_name,
						TMP_FILE_STRING);
				printf("cmd string:%s\n", CmdStr);
#ifdef DEBUG
				vLog("cmd string:%s", CmdStr);
#endif
				system(CmdStr);

#ifndef PEOPLEBANK
				memset(temName, 0x00, FILE_PATH_CHARNUM);
				/*商行自动终端文件名修改*/
				if ((tp == ATM_FILE_TYPE )|| (tp == CRS_FILE_TYPE ))
				{
					if (ReadAtmFileName(dirlist->d_name, &fn,&business)==SUCESS)
					{
						char percode[50] = { 0 };
						GetPercode(fn.Percode, percode);
						char *sub = strstr(dirlist->d_name,ATM_SEPARATOR_STRING);
#ifdef DEBUG
						vLog("bankno[%s] agencyno[%s] percode[%s] sub[%s]",
								fn.BankNo, fn.AgencyNo, fn.Percode, sub);
#endif
						sprintf(temName, "%s_%s_%s%s", fn.BankNo,
								fn.AgencyNo, percode, sub);
					}
				} else {
					sprintf(temName, "%s", dirlist->d_name);
				}
				sprintf(CmdStr, "mv %s/%s.%s %s/%s", temTarget, dirlist->d_name,
				TMP_FILE_STRING, temTarget, temName);
#else  /*人行*/
				sprintf(CmdStr, "mv %s/%s.%s %s/%s",temTarget, dirlist->d_name,TMP_FILE_STRING,
						temTarget, dirlist->d_name);
#endif

				printf("cmd string:%s\n", CmdStr);
#ifdef DEBUG
				vLog("cmd string:%s", CmdStr);
#endif
				system(CmdStr);

#ifdef PEOPLEBANK
			}
#endif
				sprintf(CmdStr, "mv %s/%s %s/%s/%s", szFolderPath,
						dirlist->d_name, g_param.FileStoreBasePath,
						g_param.InsertDir, dirlist->d_name);
				printf("cmd string:%s\n", CmdStr);
#ifdef DEBUG
				vLog("cmd string:%s", CmdStr);
#endif
				system(CmdStr);
			}
		}
		if (p != NULL) {
			closedir(p);
			p = NULL;
		}
		sleep(10);
	}

	if (th != NULL)
	{
		memset(th, 0x00, sizeof(pthread_t));
	}
	return (void *) 0;
}
void *ListDirThread(void *pt)
{
	DIR* p;
	struct dirent* dirlist;
	struct stat filestat;
	char szFolderPath[FILE_PATH_CHARNUM] = { 0 };
	char indir[FILE_PATH_CHARNUM] = { 0 };

	int SLEEPTIME = 1;

	pthread_t *th = (pthread_t *) pt;

	sprintf(szFolderPath, "%s/%s/", g_param.FileStoreBasePath,g_param.InsertDir);

#ifdef   DEBUG
	vLog("ListDirThread:%s", szFolderPath);
#endif

	while (1) {
		if (is_date_cut() == SUCESS) {
			vLog("ListDirThread:sleep in datecut!");
			sleep(60 * 10);
			continue;
		}

#ifdef   DEBUG
		vLog("ListDirThread  opendir");
#endif
		p = opendir(szFolderPath);
		if (p == NULL) {
			printf("ListDirThread Open dir %s fail:%s\n", szFolderPath,
					strerror(errno));
			vLog("ListDirThread Open dir %s fail:%s", szFolderPath,
					strerror(errno));
			return (void *) ERROR;
		}
		char *fileType;
		DataType *df;
		int fs = 0;
		int tp;
		while ((dirlist = readdir(p)) != NULL) {
			sprintf(indir, "%s/%s", szFolderPath, dirlist->d_name);
			stat(indir, &filestat);

			if (S_ISREG(filestat.st_mode)) {
				fileType = strrchr(dirlist->d_name, '.');
				if (((tp = CheckFileType(fileType)) == ERROR)
						|| tp == START_FILE_TYPE) {
					continue;
				}
				df = (DataType *) malloc(sizeof(DataType));
				memset(df, 0x00, sizeof(DataType));
				memcpy(df->filePath, szFolderPath, strlen(szFolderPath));
				memcpy(df->fileName, dirlist->d_name, strlen(dirlist->d_name));
				df->filesize = filestat.st_size;
				df->fileType = tp;

				if (StartFile(df) != SUCESS) {
					free_data_cir_queue(&df);
					continue;
				}

				push_cir_queue(&gQue, df);

#ifdef   DEBUG
				vLog("ListDirThread add:%d,queue:%d", ++fs, gQue.count);
#endif
			}

		}

#ifdef   DEBUG
		vLog("ListDirThread  sleep");
#endif
		if (p != NULL) {
			closedir(p);
			p = NULL;
		}
		sleep(10);
	}

	if (th != NULL) {
		memset(th, 0x00, sizeof(pthread_t));
	}
	return (void *) SUCESS;
}

void *HandleFileThread(void *pt) {
	DataType *df;

	int id = (int) pt;

	TRY_CONNECT_DB

	vLog("HandleFileThread[%d] starting",id);

	while (1) {
		if (is_date_cut() == SUCESS) {
			vLog("HandleFileThread:sleep in datecut!");
			sleep(60 * 10);
			continue;
		}
#ifdef DEBUG
		vLog("HandleFileThread[%d] pop_cir_queue ",id);
#endif
		df = pop_cir_queue(&gQue);

		vLog("Start file:%s  Insert.", df->fileName);
		df->threadid = id;
		int iRet;
		switch (df->fileType) {
		case FSN_FILE_TYPE:
			iRet = FSNFile(df);
			break;
		case ZIP_FILE_TYPE:
			iRet = ZIPFile(df);
			break;
		case BF_FILE_TYPE:
			iRet = BFFile(df);
			break;
		case BK_FILE_TYPE:
			iRet = BKFile(df);
			break;
		case CT_FILE_TYPE:
			break;
#ifndef PEOPLEBANK
		case DK_FILE_TYPE:
			iRet = DKFile(df);
			break;
#endif
		default:
			vLog("File type error:%d", df->fileType);
		}

		vLog("Finish_file:%s  code[%d] Insert.", df->fileName, iRet);
		CheckResult(df, iRet);

		free_data_cir_queue(df);

	}
	return (void *) 0;
}

void *SendFileThread() {

	vLog("SendFileThread   start");

	TRY_CONNECT_DB

	int iRet;
	pthread_t service;

	/*恢复初始状态*/
	DbRoute(DBS_UPDATE1, NULL);

	while (1) {
		ROUTE route;
		ROUTE *ro;
		unsigned int time = GetTimeInt();
		unsigned int date = GetDateInt();
		memset(&route,0x00,sizeof(ROUTE));
		route.starttime = time;
		route.lastdate = date;
		vLog("starttime[%d],latdate[%d]",route.starttime,route.lastdate);
		iRet = DbRoute(DBS_CURSOR_OPEN, &route);
		if (iRet != SUCESS) {
			vLog("DbRoute   DBS_CURSOR_OPEN error");
		}
        int co=0;
		while (1) {
			ro=(ROUTE *)malloc(sizeof(ROUTE));
			memset(ro,0x00,sizeof(ROUTE));
			iRet = DbRoute(DBS_FETCH, ro);
			if (iRet == NODATA || iRet == ERROR) {
#ifdef DEBUG
				vLog("DbRoute   DBS_FETCH NODATA [%d]",iRet);
#endif
				DbRoute(DBS_CLOSE, NULL);
#ifdef DEBUG
				vLog("DbRoute   DBS_CLOSE CURSOR ");
#endif
				break;
			}

			pthread_create(&service, &attr, SendTask, (void *) (ro));
		}
		sleep(60);
	}

	return (void *) 0;
}
void printRout(ROUTE *route )
{
	vLog("SendTask start:route->id[%d],route->ipaddr[%s],route->port[%s],route->user[%s]\
			route->password[%s],route->localdir[%s],route->remotedir[%s]",route->id
			,route->ipaddr,route->port,route->user,route->password
			,route->localdir,route->remotedir);

}
void *SendTask(void *sp) {
	ROUTE *ro = (ROUTE *) sp;
	ROUTE route;
	memset(&route,0x00,sizeof(ROUTE));
	memcpy(&route,ro,sizeof(ROUTE));

	free(ro);

	char tBank[20 + 1] = { 0 };
	char fBank[20 + 1] = { 0 };

	printRout(&route);

	if (ThreadConnectDB() != SUCESS) {
		return (void *) ERROR;
	}

	Start_service(&route);
	if (strlen(route.ipaddr) <= 0) {
		vLog("route[%d]  ipaddr [%s] is null ", route.id, route.ipaddr);
		Stop_service(&route);
		DisconnectDB();
		return (void *) ERROR;
	}

	if (strlen(route.localdir) <= 0 || access(route.localdir, 0) != 0) {
		vLog("route[%d] can't access localdir [%s] ", route.id,
				route.localdir);
		Stop_service(&route);
		DisconnectDB();
		return (void *) ERROR;
	}

	if (strlen(route.forwardbank) > 0) {
		memcpy(fBank, route.forwardbank, strlen(route.forwardbank));
	} else {
		if (strlen(route.targetbank) > 0) {
			memcpy(fBank, route.targetbank, strlen(route.targetbank));
		} else {
			vLog("route[id = %d] get forwardbank error ", route.id);
			Stop_service(&route);
			DisconnectDB();
			return (void *) ERROR;
		}
	}
	if (strlen(route.targetbank) > 0) {
		memcpy(tBank, route.targetbank, strlen(route.targetbank));
	} else {
		memcpy(tBank, route.forwardbank, strlen(route.forwardbank));
	}

	int iRet;
	switch (route.type) {
	case FTP_UPLOAD:
	case SFTP_UPLOAD:
		iRet = UploadFile(&route);
		break;
	case FTP_DOWNLOAD:
	case SFTP_DOWNLOAD:
		iRet = DownFile(&route);
		break;
	default:
		vLog("route[id = %d] get type [%d] error ", route.id, route.type);
		Stop_service(&route);
		DisconnectDB();
		return (void *) 0;
	}
	vLog("SUCESS route[id = %d] get interval [%d] iRet[%d] ", route.id, route.interval,iRet);
	if(iRet == SUCESS){

		if(route.interval!=0){
			route.starttime = GetTimeInterval(route.interval);
		}else{
			route.lastdate = GetDateInt();
		}
		vLog("SUCESS route[id = %d] get interval [%d]  ", route.id, route.interval);
	}else{
		vLog("SendTask error:id[%d]",route.id);
	}
	Stop_service(&route);
	DisconnectDB();
	return (void *) 0;
}

int GetBanknoAndAgencyno(const char *szFileName, FILENAME *pfilename) {
	char strFileName[FILE_PATH_CHARNUM] = { 0 };
	char *pLeftStr = NULL;
	char *pReturn1;

	strcpy(strFileName, szFileName);

	/*获取银行号*/
	pReturn1 = strtok_r(strFileName, SEPARATOR_STRING, &pLeftStr);
	if ((pReturn1 != NULL) && (BANKNO_LEN == strlen(pReturn1))) {
		memcpy(pfilename->BankNo, pReturn1, BANKNO_LEN);
	} else {
		return ERROR;
	}
	/*获取网点号*/
	pReturn1 = strtok_r(NULL, SEPARATOR_STRING, &pLeftStr);
	if ((pReturn1 != NULL) && (AGENCYNO_LEN == strlen(pReturn1))) {
		memcpy(pfilename->AgencyNo, pReturn1, AGENCYNO_LEN);
	} else {
		return ERROR;
	}
	return SUCESS;
}

/*恢复上次退出未处理完的文件为初始状态*/
void CoverFiles() {
	DIR* p;
	struct dirent* dirlist;
	struct stat filestat;
	char szFolderPath[FILE_PATH_CHARNUM] = { 0 };
	char indir[FILE_PATH_CHARNUM] = { 0 };

	sprintf(szFolderPath, "%s/%s/", g_param.FileStoreBasePath,
			g_param.InsertDir);

#ifdef   DEBUG
	vLog("InsertDir:%s", szFolderPath);
#endif
	p = opendir(szFolderPath);
	if (p == NULL) {
		printf("CoverFiles Open dir %s fail:%s\n", szFolderPath,
				strerror(errno));
		vLog("CoverFiles Open dir %s fail:%s", szFolderPath, strerror(errno));
		return;
	}
	char *fileType;
	DataType *df;
	int tp;
	while ((dirlist = readdir(p)) != NULL) {
		sprintf(indir, "%s/%s", szFolderPath, dirlist->d_name);
		stat(indir, &filestat);
		if (S_ISREG(filestat.st_mode)) {
			fileType = strrchr(dirlist->d_name, '.');
			if ((tp = CheckFileType(fileType)) != START_FILE_TYPE) {
				continue;
			}
			df = (DataType *) malloc(sizeof(DataType));
			memset(df, 0x00, sizeof(DataType));
			memcpy(df->filePath, szFolderPath, strlen(szFolderPath));
			memcpy(df->fileName, dirlist->d_name,
					strlen(dirlist->d_name) - strlen(fileType));
			df->fileType = tp;
			df->filesize = filestat.st_size;
			if (StopFile(df) != SUCESS) {
				free_data_cir_queue((void *) df);
				continue;
			}
		}

	}

	if (p != NULL) {
		closedir(p);
		p = NULL;
	}

}

int CheckFileType(char *fileType)
{
	if (strstr(fileType, FSN_FILE_STRING) != 0) {
		return FSN_FILE_TYPE;
	} else if (strstr(fileType, ZIP_FILE_STRING) != 0) {
		return ZIP_FILE_TYPE;
	} else if (strstr(fileType, BF_FILE_STRING) != 0) {
		return BF_FILE_TYPE;
	} else if (strstr(fileType, BK_FILE_STRING) != 0) {
		return BK_FILE_TYPE;
	} else if (strstr(fileType, CT_FILE_STRING) != 0) {
		return CT_FILE_TYPE;
	} else if (strstr(fileType, START_FILE_STRING) != 0) {
		return START_FILE_TYPE;
	}
#ifndef  PEOPLEBANK
	else if (strstr(fileType, DK_FILE_STRING) != 0) {
		return DK_FILE_TYPE;
	}
#endif
	return ERROR;
}

void CheckResult(DataType *df, int result) {
	int iRet;
	int st = 1;
	switch (result) {
	case ERROR:
		if (df->fileType == ZIP_FILE_TYPE) {
			StopFile(df);
#ifdef   DEBUG
			vLog("ZIP_FILE_TYPE error stop :%s", df->fileName);
#endif
		}
		break;
	case SUCESS:
		break;
	case WARING: {
		StopFile(df);
		iRet = TestConnectionDB();
		if (iRet == ERROR) {
			DisconnectDB();
			while (ThreadConnectDB() != SUCESS) {
				sleep(10 * st);
				st = (st + 1) % 200;
			}
		}
		vLog("Reconnect db ok!");
		return;
	}
	}

}

int MoveFiles(FILEMOVEINFO *pInfo) {
	char CmdStr[500] = { 0 };
	char ShellName[FILE_PATH_CHARNUM] = { 0 };
	char pCurPath[FILE_PATH_CHARNUM] = { 0 };

	if (pInfo->isRemote == 1) { //文件需要远程移动
#if 0
			if (0 == strcmp(g_param.PassMode, "2"))	//SFTP
			{
				strcpy(ShellName, SFTP_PUTSHELLNAME_STRING);
			} else								  //FTP
			{
				strcpy(ShellName, FTP_PUTSHELLNAME_STRING);
			}

			memset(CmdStr,0x00,sizeof(CmdStr));
			sprintf(CmdStr,"mv %s/%s %s%s.tmp",pInfo->FilePth,pInfo->FileBaseName,pInfo->FilePth,pInfo->FileBaseName);
			if(system(CmdStr)!=0) {
				return -1;
			}
			memset(CmdStr,0x00,sizeof(CmdStr));
			sprintf(CmdStr, "%s/%s %s %s %s %s.tmp %s.tmp %s", pCurPath, ShellName,
					pInfo->ReIp, pInfo->ReFilePath, pInfo->FilePth,
					pInfo->FileBaseName,pInfo->FileBaseName,pInfo->ReFileBaseName);

			printf("cmd string:%s\n", CmdStr);
			sprintf(g_LogWords, "cmd string:%s", CmdStr);
			WriteLog(g_LogWords);

			int iPutFileRet = system(CmdStr);	//调用shell脚本
			printf("----------->iPutFileRet:%d\n", iPutFileRet);
			sprintf(g_LogWords, "iPutFileRet:%d", iPutFileRet);
			WriteLog(g_LogWords);

			//put完后判断put成功
			int iPutMoveFileRet = JudgeRemotePutMoveFileOk(pInfo->FileBaseName,
					pCurPath);
			if (0 != iPutMoveFileRet) {
				memset(CmdStr,0x00,sizeof(CmdStr));
				sprintf(CmdStr,"mv %s/%s.tmp %s%s",pInfo->FilePth,pInfo->FileBaseName,pInfo->FilePth,pInfo->FileBaseName);
				system(CmdStr);
				sprintf(g_LogWords, "remote move file:%s/%s err\n",pInfo->FilePth,pInfo->FileBaseName);
				WriteLog(g_LogWords);
				return -1;
			}
#endif
	}

	char oldname[MAX_STRING_SIZE];
	char newname[MAX_STRING_SIZE];
	sprintf(newname, "%s/%s", pInfo->LocalDesFilePath, pInfo->LocalDesBaseName);
	sprintf(oldname, "%s/%s", pInfo->FilePth, pInfo->FileBaseName);

#ifdef DEBUG
	vLog("newname[%s] oldname[%s]\n", newname, oldname);
#endif

	if (rename(oldname, newname) == 0) {
		if (pInfo->isCompress) {
			sprintf(oldname, "%s", newname);
			sprintf(newname, "%s/%s.zip", pInfo->LocalDesFilePath,
					pInfo->LocalDesBaseName);
			CompressFile(oldname, newname);
			remove(oldname);
		}
		return SUCESS;
	}
	return ERROR;
}

int StartFile(DataType *df) {
	char oldname[MAX_STRING_SIZE];
	char newname[MAX_STRING_SIZE];
	sprintf(oldname, "%s/%s", df->filePath, df->fileName);
	sprintf(newname, "%s/%s.%s", df->filePath, df->fileName, START_FILE_STRING);
	if (rename(oldname, newname) == 0) {
		return SUCESS;
	}
	return ERROR;
}

int StopFile(DataType *df) {
	char oldname[MAX_STRING_SIZE];
	char newname[MAX_STRING_SIZE];
	sprintf(newname, "%s/%s", df->filePath, df->fileName);
	sprintf(oldname, "%s/%s.%s", df->filePath, df->fileName, START_FILE_STRING);
	if (rename(oldname, newname) == 0) {
		return SUCESS;
	}
	return ERROR;
}
int CheckSortCenter(char *bankno) {
	return SUCESS;
}
char *GetErrorLocalDir(char *dir, const FILENAME *fn) {
	char temDir[FILE_PATH_CHARNUM] = { 0 };
	char newDate[9] = { 0 };
	GetGiveDate(newDate);
	sprintf(temDir, "%s/%s/%s/%s/", g_param.FileStoreBasePath, g_param.ErrorDir,
			g_param.BankNo, newDate);
	memcpy(dir, temDir, strlen(temDir));
	JudgeSavePathExist(temDir);
	return dir;
}
