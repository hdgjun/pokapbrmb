/*
 * shellftp.c
 *
 *  Created on: 2016年12月27日
 *      Author: Administrator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "pokalog.h"
#include "pokaftp.h"

static char *g_FTP_model[4]={"mput","mget","put","get"};

int shellftp1(FTP_MESSAGE *msg)
{
	FILE *fp;
	int rc = 0; /*管道关闭返回码*/
	char acBuf[10240];
	memset(acBuf, 0x00, sizeof(acBuf));
	FILE *fp1;/*读取标准输出信息文件指针*/

	fprintf(stderr,"acBuf\n");
	//chdir(msg->localdir);


	char cmd[100] ={0};
	sprintf(cmd,"ftp -n -v|tee %s",msg->logname);
	/*打开FTP传输管道*/
	if ((fp = popen(cmd, "w")) == NULL) {
		//vLog("Open FTP popen error!");
		return ERROR;
	}
/*
	if ((fp1 = freopen(msg->logname, "r", stdout)) == NULL)
	{
			vLog("error redirecting  stdout!");
			fprintf(stderr,"error");
	}*/
	int iRet;
	iRet = fprintf(fp, "open %s %s\n", msg->ip,msg->port);
	vLog("open [%d]",iRet);
	iRet = fprintf(fp, "user %s %s\n", msg->user, msg->passwd);
	vLog("user [%d]",iRet);
	fprintf(fp, "prompt off\n");
	fprintf(fp, "bin\n");
	fprintf(fp, "lcd %s\n",msg->localdir);
	fprintf(fp, "cd %s\n", msg->remotedir);
	fprintf(fp, "%s %s\n", g_FTP_model[msg->model], msg->filename); /*文件传输*/
	fprintf(fp, "bye\n");

/*	while ((fgets(acBuf, sizeof(acBuf) - 1, fp)) != NULL) {
		vLog("ftp:%s",acBuf);
	}
	fclose(fp1);*/

	/*关闭管道取得shell命令执行返回码 -1：管道关闭失败 大于0：命令执行失败*/
	rc = pclose(fp);
	//vLog("FTP code [%d]", rc);
	if (rc == -1) {
		//vLog("close FTP popen error");
		return ERROR;
	} else if (rc > 0) {
		//vLog("FTP err code [%d]", rc);
		return ERROR;
	}
	return SUCESS;
}

int Openftp(FILE *fp,FTP_MESSAGE *msg)
{
	int iRet;
	if((fp = popen("ftp -n -v", "w")) == NULL) {
			vLog("Open FTP popen error!");
			return ERROR;
	}

	fprintf(fp, "open %s %s\n", msg->ip,msg->port);
	fprintf(fp, "user %s %s\n", msg->user, msg->passwd);
	fprintf(fp, "prompt off\n");
	fprintf(fp, "bin\n");

	return SUCESS;
}
int Sendftp(FILE *fp,FTP_MESSAGE *msg)
{
//	chdir(msg->localdir);

//	fprintf(fp, "cd %s\n", msg->remotedir);
//	fprintf(fp, "%s %s\n", g_FTP_model[msg->model], msg->filename);
	char ldir[200] = {0};
	switch(msg->model){
		case FTP_MPUT:

			//char rdir[200] = {0};
			sprintf(ldir,"%s/%d/",msg->localdir,(unsigned int)pthread_self());
			JudgeSavePathExist(&ldir);
			fprintf(fp, "cd %s\n", msg->remotedir);
			fprintf(fp, "lcd %s\n", ldir);
			fprintf(fp, "%s %s\n", g_FTP_model[msg->model], msg->filename);
			break;
		case FTP_MGET:
			break;
		case FTP_PUT:
			break;
		case FTP_GET:
			break;
	}
	return SUCESS;
}
int Closeftp(FILE *fp,FTP_MESSAGE *msg)
{
	int rc;

	fprintf(fp, "bye\n");
	rc = pclose(fp);

	if (rc == -1) {
		return ERROR;
	} else if (rc > 0) {
		return ERROR;
	}
	return SUCESS;
}

