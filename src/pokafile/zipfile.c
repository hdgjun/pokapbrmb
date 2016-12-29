#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pokafile.h"
#include "switch.h"

int ZIPFile(DataType *df)
{
	char cmd[MAX_STRING_SIZE] = {0};
	char udir[CUR_STRING_SIZE] = {0};
	char srcfile[CUR_STRING_SIZE] = {0};
	int iRet = SUCESS;

	sprintf(udir,"%s/%s_%s/", df->filePath,df->fileName,START_FILE_STRING);
	sprintf(srcfile,"%s/%s.%s", df->filePath,df->fileName,START_FILE_STRING);
	sprintf(cmd, "unzip -o %s -d %s", srcfile,udir);
	#ifdef DEBUG
		vLog("unzip cmd:%s",cmd);
	#endif
	system(cmd);
	if(access(udir,0)!=0){
	#ifdef DEBUG
		vLog("unzip udir not exit:%s",udir);
	#endif
		return ERROR;
	}
	memset(cmd,0x00,MAX_STRING_SIZE);
	sprintf(cmd, "mv %s/* %s", udir,df->filePath);
	system(cmd);
	if(CheckDirEmpty(udir) == ERROR){
		iRet = ERROR;
	}
	memset(cmd,0x00,MAX_STRING_SIZE);
	sprintf(cmd, "rm -rf %s/", udir);
	system(cmd);

    if(iRet == SUCESS){
		memset(cmd,0x00,MAX_STRING_SIZE);
		sprintf(cmd, "rm -rf %s", srcfile);
		system(cmd);
    }

	return iRet;
}

int CheckDirEmpty(char *dir)
{
	DIR* p;
	struct dirent* dirlist;
	struct stat filestat;
	char indir[FILE_PATH_CHARNUM] = { 0 };
	p = opendir(dir);
	if (p == NULL) {
		return SUCESS;
	}
	int iRet = SUCESS;
	while ((dirlist = readdir(p)) != NULL) {
		sprintf(indir, "%s/%s", dir, dirlist->d_name);
		stat(indir, &filestat);
		if (S_ISREG(filestat.st_mode)) {
			if (p != NULL) {
				closedir(p);
				p = NULL;
			}
			return ERROR;
		}
	}
	if (p != NULL) {
		closedir(p);
		p = NULL;
	}
	return iRet;
}
char *CompressFile(char *srcFile,char *zipFile)
{
	char cmd[MAX_STRING_SIZE] = {0};
	sprintf(cmd,"%s -rj %s %s",ZIPCMD_STRING,zipFile,srcFile);
	printf("CompressFile CompressStr:%s\n",zipFile);
#ifdef DEBUG
	vLog("CompressFile CompressStr:%s\n",zipFile);
#endif
	system(cmd);//÷¥––—πÀı√¸¡Ó
	return zipFile;
}
