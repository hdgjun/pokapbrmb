/*
 * pokapbrmb.h
 *
 *  Created on: 2016年6月15日
 *      Author: Administrator
 */

#ifndef _POKAPBRMB_H_
#define _POKAPBRMB_H_

#define APP_NAME "pokapbrmb"
#define APP_VERSION   "V2.1.20161220"
#define POKA_HOME              "POKAHOME"
#define SHELL_DIR              "sbin"
#define CFG_DIR                "cfg"
#define LOGFOLDER_NAME		   "log"					//日志存储目录名

pthread_attr_t attr;

char *GetProgramPath(char *path);
#endif /* POKAPBRMB_H_ */
