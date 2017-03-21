#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "switch.h"
#include "common.h"
#include "filepath.h"


int DelFileOrDir(unsigned int uiFileDirFlag,const char *szFileDirPath)
{
	DIR *pdir = NULL;
	FILE *pfile = NULL;
	int iExecSysRet = 0;
	char strDelCmd[500] = {0};

	switch(uiFileDirFlag)
	{
		case 0://目录
			pdir = opendir(szFileDirPath);
			sprintf(strDelCmd,"rm %s -rf",szFileDirPath);
			if(pdir == NULL)
			{
				return ERROR;
			}
			closedir(pdir);
			pdir = NULL;
			break;
		case 1://文件
			pfile = fopen(szFileDirPath,ADD_ACCESS_STRING);
			sprintf(strDelCmd,"rm %s",szFileDirPath);
			if(pfile == NULL)
			{
				return ERROR;
			}
			fclose(pfile);
			pfile = NULL;
			break;
		default:
			return ERROR;
	}

	iExecSysRet = my_system(strDelCmd);
	if(iExecSysRet == -1)
	{
		printf("Delete %s fail:%s\n",szFileDirPath,strerror(errno));
	}
	else
	{
		printf("Delete %s ok\n",szFileDirPath);
	}

	return SUCESS;
}

int Move(const char *src,const char *des)
{
	char cmd[1024] = {0};
	sprintf(cmd,"mv %s %s",src,des);

	printf("upload_put_cmd:%s\n",cmd);
	return my_system(cmd);
}

int GetTimeInterval(int sec)
{
	char ti[6+1] = {0};
	time_t	NowTime;	// 当前时间数
	struct	tm now;	// 当前时间结构体
	NowTime = time(NULL);
	NowTime += sec;// 3个小时前的时间数
	localtime_r(&NowTime, &now);
	sprintf(ti,"%2d%02d%02d", now.tm_hour,
			now.tm_min+1,
			now.tm_sec);
	return atoi(ti);
}
#if 0
int getInterval(time_t *oldt)
{
	time_t ntime;
	char strNow[12+1] = {0};
	char strOld[12+1] = {0};

	ntime = time(0);
	struct tm tnow = {0};

	localtime_r(&ntime,&tnow);
	sprintf(strNow,"%4d%02d%02d%02d%02d%02d",1900+tnow.tm_year,
			tnow.tm_mon+1,tnow.tm_mday,
			tnow.tm_hour,tnow.tm_min,tnow.tm_sec);

	struct tm told = {0};
	localtime_r(oldt,&told);

	sprintf(strOld,"%4d%02d%02d%02d%02d%02d",1900+told.tm_year,
			told.tm_mon+1,told.tm_mday,
			told.tm_hour,told.tm_min,told.tm_sec);
	printf("old time[%s],cur time[%s]\n",strOld,strNow);
	return atoi(strNow) - atoi(strOld);
}
#endif

int getInterval(time_t oldt)
{
	time_t ntime;

	ntime = time(0);

	printf("old time[%d],cur time[%d]\n",oldt,ntime);
	return ntime - oldt;
}

int GetDateInterval(int sec)
{
	char ti[8+1] = {0};
	time_t	NowTime;	// 当前时间数
	struct	tm now;	// 当前时间结构体
	NowTime = time(NULL);
	NowTime += (sec*60*60*24);
	localtime_r(&NowTime, &now);

	sprintf(ti,"%4d%02d%02d", 1900+now.tm_year,
			now.tm_mon+1,
			now.tm_mday);
	return atoi(ti);
}
int GetTimeInt()
{
	char time[6+1] = {0};
	GetTime(time);
	return atoi(time);
}
char *GetTime(char *strTime){
	time_t now;
	now = time(0);
	struct tm tnow = {0};
	localtime_r(&now,&tnow);
	sprintf(strTime,"%2d%02d%02d", tnow.tm_hour,
			tnow.tm_min,
			tnow.tm_sec);
	return strTime;
}

int GetDateInt()
{
	char date[8+1] = {0};
	GetNowDate(date);
	return atoi(date);
}
char *GetGiveDate(char *strDate)
{
	time_t now;
	now = time(0);
	struct tm *tnow = localtime(&now);
	sprintf(strDate,"%4d%02d%02d", 1900+tnow->tm_year,
			tnow->tm_mon+1,
			tnow->tm_mday);

	return strDate;
}

char *GetNowDate(char *strDate)
{
	time_t now;
	now = time(0);
	struct tm tnow = {0};
	localtime_r(&now,&tnow);
	sprintf(strDate,"%4d%02d%02d", 1900+tnow.tm_year,
			tnow.tm_mon+1,
			tnow.tm_mday);

	return strDate;
}

//十六进制转化成十进制
int htoi(char *s)
{
	int i, n;
	n = 0;
	for (i = 0; s[i] != '\0'; i++) {
			if (s[i] >= '0' && s[i] <= '9')
					n = n * 16 + s[i] - '0';
			if (s[i] >= 'a' && s[i] <= 'f')
					n = n * 16 + s[i] - 'a' + 10;
			if (s[i] >= 'A' && s[i] <= 'F')
					n = n * 16 + s[i] - 'A' + 10;
	}
	return n;
}

int htoi1(char *s)
{
	int i,n;
	n=0;
	for(i=0;s[i]!='\0';i++)
	{ if(s[i]>='0'&&s[i]<='9')  n=n*16+s[i]-'0';
	if(s[i]>='a'&&s[i]<='f')  n=n*16+s[i]-'a'+10;
	if(s[i]>='A'&&s[i]<='F')  n=n*16+s[i]-'A'+10;
	}
	return n;
}

//去除空格
char *strtrim(char *pstr)
{
	char *p;

	p = strtrimr(pstr);
	return strtriml(p);
}

char *strtrimr(char *pstr)
{
    char *p;
	p = pstr;
	int i;
	i = strlen(pstr) - 1;
	while((i >= 0) && isspace(p[i]))
	{
		p[i--] = '\0';
	}
	return pstr;
}

//整形版本指数函数
int npow(int n,int m)
{
    int sum = 1;
	int i = 0;
    for(i = 0; i < m; i++)
	{
        sum *= n;
    }

    return sum;
}

char *strtriml(char *pstr)
{
	int i = 0;
	int j = 0;
	j = strlen(pstr) - 1;
	while((i <= j) && isspace(pstr[i]))
	{
		i++;
	}
	if(i > 0)
	{
		strcpy(pstr,&pstr[i]);
	}
	return pstr;
}

//整型转化成字符串函数
void itoafunc(int i, char*string)
{
		int power, j;
		j = i;
		for (power = 1; j >= 10; j /= 10)
				power *= 10;
		for (; power > 0; power /= 10) {
				*string++ = '0' + i / power;
				i %= power;
		}
		*string = '\0';
}

int JudgeSavePathExist(char *szFolderPath)
{
	//创建指定目录
	DIR *pDir = NULL;
	pDir = opendir(szFolderPath);
	char sz[FILE_PATH_CHARNUM] = {0};
	if (pDir == NULL)
	{
		sprintf(sz, "%s%s", "mkdir -m 755 -p ", szFolderPath);
		printf("sz = %s\n", sz);
		return my_system(sz);
	}
	else
	{
		closedir(pDir);
		pDir = NULL;
		return SUCESS;
	}
}

