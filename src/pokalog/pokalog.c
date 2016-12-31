#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>	//Ïß³Ì
#include <sys/types.h>
#include <sys/time.h>
#include <semaphore.h>

#include "switch.h"
#include "common.h"
#include "pokalog.h"

#if 0
sem_t log_sem;
#endif

char G_LOGPATH[FILE_PATH_CHARNUM];

void vWriteStrFile( char *psLogFileName,  char *pszMsg );
static char *GetNowDateLogName(char *logName);


int initPokaLog()
{
#if 0
	int res;

	/* Create semaphore */
	res = sem_init(&log_sem, 0, 2);
	if (res != 0)
	{
		return ERROR;
	}
#endif
	return SUCESS;
}
/**************************************************************************/
/*FUNCTION NAME: vLogErrReal.                                             */
/*DESCRIPTION:   write string log until NULL terminated                   */
/*PARAMETER:     vspFileName  - input,  which file use this log FUNCTION  */
/*               vspFuncName  - input,  which FUNCTION in the file        */
/*                                         to use this log FUNCTION       */
/*               vtLine   - input, which line ,                           */
/*               vtLevel  - input, log level eg:DBUG,NOMAL,WARRNING,ERROR */
/*               vspFmt   - input, source data ,                          */
/**************************************************************************/
void vLogErrReal( char const *vspFileName, char const *vspFuncName, int vtLine,int vtLevel,char *vspFmt,...)
{
    va_list     strArgPtr;
    time_t      strTime;
    /* long  lTimeTick ; */
    struct tm   *strpTm;
    /*struct tms    tTMS;*/
    int   nMsgOutLen = 0;

    struct timeval currTime;

    char sMsg[4096];
    char logName[FILE_PATH_CHARNUM/2];
    char logPath[FILE_PATH_CHARNUM];

    memset(sMsg, 0x00, sizeof(sMsg));
    memset(logName,0x00,sizeof(logName));
    memset(logPath,0x00,sizeof(logPath));

    GetNowDateLogName(logName);
    sprintf(logPath,"%s/%s",G_LOGPATH,logName);

    time(&strTime);
    strpTm = localtime(&strTime);
    gettimeofday(&currTime, NULL);

    va_start( strArgPtr, vspFmt );
     sprintf(sMsg, "[%04d-%02d-%02d %02d:%02d:%02d:%u][%u][%s:%d]",
            strpTm->tm_year+1900,
            strpTm->tm_mon+1,
            strpTm->tm_mday,
            strpTm->tm_hour,
            strpTm->tm_min,
            strpTm->tm_sec,
			(unsigned int)currTime.tv_usec,
			(unsigned int)pthread_self(),
            vspFileName,
            vtLine );

    nMsgOutLen = strlen(sMsg);
    nMsgOutLen += vsprintf(sMsg+nMsgOutLen, vspFmt, strArgPtr);

    va_end(strArgPtr);

    vWriteStrFile(logPath,sMsg);
    return;
}

/**************************************************************************/
/*FUNCTION NAME: vWriteStrFile.                                           */
/*DESCRIPTION:   write string log until NULL terminated                   */
/*PARAMETER:     psLogFileName  - input, log name                         */
/*               pszMsg   - input, source data until NULL terminated,     */
/**************************************************************************/
void vWriteStrFile( char *psLogFileName,  char *pszMsg )
{
    FILE *fp=NULL;
    fp=fopen(psLogFileName,"a+");
    if(NULL==fp)
      return;

    fprintf( fp,"%s",pszMsg );
    fprintf( fp, "\n");

    fflush(fp);
    fclose(fp);
    return;
}

static char *GetNowDateLogName(char *logName)
{
	time_t now;

	now = time(0);
	struct tm *tnow = localtime(&now);

	sprintf(logName,"%4d%02d%02d%s", 1900+tnow->tm_year,
			tnow->tm_mon+1,
			tnow->tm_mday,
			TXTFILE_TYPE_STRING);
	return logName;
}

