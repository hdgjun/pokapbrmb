/*
 * cleanfile.c
 *
 *  Created on: 2016Äê12ÔÂ30ÈÕ
 *      Author: Administrator
 */
#include "common.h"
#include "pokafile.h"
#include "pokalog.h"
#include "pokapbrmb.h"
#include "switch.h"

void *CleanFileThread() {
	char DelQcfStr[256] = { 0 };
	char date[9] = { 0 };
	int idate;
	char apppath[256] = {0};
	while (1) {
#ifdef DEBUG
		vLog("**************************************************************************************");
#endif
		GetProgramPath(apppath,POKA_HOME,DEF_INSTALL_PATH);


		sprintf(DelQcfStr, "find %s/log/ -name *.txt -type f -ctime +%d -exec rm -f {} \\;>/dev/null 2>&1 ", apppath, g_param.LogSaveDay);
#ifdef DEBUG
		vLog("***cmd[%s]",DelQcfStr);
#endif
		vLog("Delete log where date < %d",g_param.LogSaveDay);
		system(DelQcfStr);

		idate = GetDateInterval(0-g_param.ImageSaveDay);
		sprintf(DelQcfStr, "sh %s/%s/%s %s/%s/ %d", apppath,SHELL_DIR,DELETE_IMAGE_SHELLNAME_STRING,g_param.ImagePath,g_param.ImageDir,idate);
#ifdef DEBUG
		vLog("***cmd[%s]",DelQcfStr);
#endif
		vLog("Delete image where date < %d",idate);
		system(DelQcfStr);
#ifdef DEBUG
		vLog("**************************************************************************************");
#endif
		idate = GetDateInterval(0-g_param.FileSaveDay);
			sprintf(DelQcfStr, "sh %s/%s/%s %s/%s/ %d", apppath,SHELL_DIR,DELETE_FSN_SHELLNAME_STRING,g_param.FileStoreBasePath,g_param.handleDir,idate);
	#ifdef DEBUG
			vLog("***cmd[%s]",DelQcfStr);
	#endif
			vLog("Delete files where date < %d",idate);
			system(DelQcfStr);
	#ifdef DEBUG
			vLog("**************************************************************************************");
	#endif
			psleep(3600*24);
	}
}
