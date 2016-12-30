#ifndef _FILEPATH_H_
#define _FILEPATH_H_

int JudgeSavePathExist(char *szFolderPath);
char *strtrim(char *pstr);
char *strtrimr(char *pstr);
char *strtriml(char *pstr);
void itoafunc(int i, char*string);
int htoi(char *s);
char *GetNowDate(char *strDate);
char *GetGiveDate(char *strDate);
char *GetTime(char *strTime);
int GetTimeInt();
int GetDateInt();
int Move(const char *src,const char *des);
int DelFileOrDir(unsigned int uiFileDirFlag,const char *szFileDirPath);
int GetTimeInterval(int sec);
int GetDateInterval(int sec);
int npow(int n,int m);
int htoi1(char *s);
#endif
