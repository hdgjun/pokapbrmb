#include <stdio.h> 
#include <time.h> 
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include "common.h"
#include "pokalog.h"

#define APP_NAME 				"pokadeamon"
#define APP_VERSION   			"V2.1.20161220"
#define TARGET                  "pokapbrmb"
#define POKA_HOME              	"POKAHOME"
#define SHELL_DIR              	"sbin"
#define BIN_DIR                 "bin"
#define CFG_DIR               	"cfg"
#define LOGFOLDER_NAME		   	"log"					//��־�洢Ŀ¼��
#define TIME_SPACE             60

#define  DEF_INSTALL_PATH           "/home/poka/pokapbrmb/"

static void init_daemon(void);//�ػ����̳�ʼ������
static int initlog();
extern char G_LOGPATH[FILE_PATH_CHARNUM];
char install[100]={0};

int main() 
{ 
	FILE *fp; 
	time_t t; 

	init_daemon();//��ʼ��ΪDaemon 
	initlog();


	int iret;
	while(1)
	{ 		
		if(SUCESS == JudgeProgramExist(0,TARGET))
		{
			vLog("Target program[%s] does not run!",TARGET);

			char szBuf[1024] = {0};
			sprintf(szBuf, "nohup %s/%s/%s &",install, BIN_DIR, TARGET);

			int iExecSysRet = system(szBuf);
			if(iExecSysRet == -1)
			{
				vLog("Target program[%s/%s] fail:%s",install,TARGET,strerror(errno));
			}
			else
			{
				vLog("Exec Target program[%s/%s] ok",install,TARGET);
			}
		}
		else
		{
			vLog("Target program[%s/%s] is running",install,TARGET);
		}
		psleep(TIME_SPACE);
	} 

	return 0;
} 

void init_daemon(void) 
{ 
	int pid; 
	int i; 
	char path[100]={0};
	GetProgramPath(install,POKA_HOME,DEF_INSTALL_PATH);

	sprintf(path,"%s/%s",install,BIN_DIR);
	if(pid=fork()) 
	exit(0);//�Ǹ����̣����������� 
	else if(pid< 0) 
	exit(1);//forkʧ�ܣ��˳� 
	//�ǵ�һ�ӽ��̣���̨����ִ�� 
	setsid();//��һ�ӽ��̳�Ϊ�µĻỰ�鳤�ͽ����鳤 
	//��������ն˷��� 
	if(pid=fork()) 
	exit(0);//�ǵ�һ�ӽ��̣�������һ�ӽ��� 
	else if(pid< 0) 
	exit(1);//forkʧ�ܣ��˳� 
	//�ǵڶ��ӽ��̣����� 
	//�ڶ��ӽ��̲����ǻỰ�鳤 

	for(i=0;i< NOFILE;++i)//�رմ򿪵��ļ������� 
	close(i); 
	chdir(path);//�ı乤��Ŀ¼��/tmp
	umask(0);//�����ļ�������ģ 
	
	return; 
} 

/**
 *��ʼ����־
 */
static int initlog()
{
    char curDir[FILE_PATH_CHARNUM] = {0};
	char LogDirPath[FILE_PATH_CHARNUM] = { 0 };

	sprintf(LogDirPath, "%s/%s/%s/", GetProgramPath(curDir,POKA_HOME,DEF_INSTALL_PATH), LOGFOLDER_NAME,APP_NAME);

	if (JudgeSavePathExist(LogDirPath) == ERROR) {
		return ERROR;
	}
	sprintf(G_LOGPATH, "%s", LogDirPath);

	return initPokaLog();
}
