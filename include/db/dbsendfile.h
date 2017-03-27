/*
 * dbsendfile.h
 *
 *  Created on: 2017Äê3ÔÂ28ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBSENDFILE_H_
#define INCLUDE_DB_DBSENDFILE_H_

typedef struct T_SendFile
{
	char filename[100+1];
	char bankno[20+1];
	char senddate[20+1];
	char checkdate[20+1];
}SENDFILE;

int DbSendFile(int oprType, SENDFILE *re);

#endif /* INCLUDE_DB_DBSENDFILE_H_ */
