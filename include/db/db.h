#ifndef _DB_H_
#define _DB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "switch.h"
#ifdef DB_ORACLE
#include <sqlca.h>
#endif
#ifdef DB_MYSQL
#include <mysql.h>

#endif
#include <pthread.h>

extern  pthread_key_t p_Thread_key;

/* dbs operation defines for first argument in dbs function */
#define DBS_INIT                  0
#define DBS_SELECT                1
#define DBS_LOCK                  2
#define DBS_UPDATE                3
#define DBS_DELETE                4
#define DBS_INSERT                5
#define DBS_TRUNCATE              6
#define DBS_EXECUTE               7
#define DBS_SEL_FOR_UP            8
#define DBS_SEL_ONE_ROW           9

#define DBS_CURSOR                11
#define DBS_OPEN                  12
#define DBS_CURSOR_OPEN           13
#define DBS_CLOSE                 14
#define DBS_FETCH                 15
#define DBS_LOCK1                 21
#define DBS_UPDATE1               22
#define DBS_DELETE1	              30
#define DBS_SELECT2               21
#define DBS_SELECT3               31
#define DBS_LOCK2                 24
#define DBS_UPDATE2               25
#define DBS_LOCK3                 27
#define DBS_UPDATE3               28
#define DBS_UPDATE4               29
#define DBS_UPDATE5               30
#define DBS_UPDATE6               31
#define DBS_UPDATE7               32
#define DBS_UPDATE8               33
#define DBS_UPDATE9               34
#define DBS_CREATE_TEMP           35

#define DBS_SELECT1               101
#define DBS_SEL_ONE_ROW1          109
#define DBS_SEL_ONE_ROW2          110

#ifdef DB_ORACLE
/*
#define  POKA_ORA  struct sqlca sqlca;\
sql_context ctx;\
ctx = (sql_context)pthread_getspecific(p_Thread_key);\
EXEC SQL CONTEXT USE :ctx;
*/
int	JudgeSqlExecResultLocal(int iFlag,char *szExecText,struct sqlca *sqlca);

#endif


#ifdef DB_MYSQL

typedef struct t_mysqlctx
{
	MYSQL *pcon;
	MYSQL_RES *result;
	MYSQL_FIELD *fd;
}MYSQLCTX;

#define POKA_MYSQL MYSQL *pcon;\
		MYSQLCTX *ctx = (MYSQLCTX *)pthread_getspecific(p_Thread_key);\
		pcon=ctx->pcon;\

int	JudgeSqlExecResultLocal(int iFlag,char *szExecText,MYSQL *pcon);
#endif

int InitDb(char *path);
int DestroyDb();
int ThreadConnectDB();
int DisconnectDB();
int TestConnectionDB();
int DbsCommit();
int DbsRollback();
int CheckSortCenter(char *bankno);

#endif
