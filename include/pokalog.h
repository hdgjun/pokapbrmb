#ifndef _POKALOG_H_
#define _POKALOG_H_

#define INFO 0
#define MESSAGE 1
#define vLog(... ) \
    vLogErrReal(__FILE__, __func__, __LINE__, MESSAGE, ##__VA_ARGS__)

#define TXTFILE_TYPE_STRING ".txt"

extern char G_LOGPATH[];

void vLogErrReal( char const *vspFileName, char const *vspFuncName, int vtLine,int vtLevel,char *vspFmt,...);
int initPokaLog();
#endif
