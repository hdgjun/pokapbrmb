#ifndef _POKA_FILE_H
#define _POKA_FILE_H
#include <time.h>
#include "allcommon.h"
#include "pokalog.h"
#include "db/dbmoneydata.h"
#include "db/dbbusinesslistatm.h"
#include "db/dbrouterule.h"
#include "db/dbroute.h"
#include "switch.h"

#define FTP_UPLOAD     0
#define FTP_DOWNLOAD   1
#define SFTP_UPLOAD    2
#define SFTP_DOWNLOAD  3

#define  FTP_SUCESS                	"sucess"
#define  FTP_ERROR                  "error"
#define  TMP_FILE_STRING          	"tmp"
#define  START_FILE_STRING          "START"
#define  FSN_FILE_STRING			"FSN"					//冠字号文件后缀
#define  ZIP_FILE_STRING			"zip"
#define  BF_FILE_STRING				"BF"					//加钞文件后缀
#define  BK_FILE_STRING				"BK"					//把捆信息文件后缀
#define  CT_FILE_STRING				"CT"					//存取款信息文件后缀
#define  ATM_FILE_STRING            "ATM"
#define  CRS_FILE_STRING            "CRS"
#define  DK_FILE_STRING             "DK"
#define  SK_FILE_STRING             "SK"

#define  FSN_FILE_TYPE				0
#define  ZIP_FILE_TYPE				1
#define  BF_FILE_TYPE				2
#define  BK_FILE_TYPE	            3
#define  CT_FILE_TYPE	            4
#define  ATM_FILE_TYPE              5
#define  DK_FILE_TYPE               6
#define  START_FILE_TYPE            7
#define  CRS_FILE_TYPE              8
#define  SK_FILE_TYPE               9

#define  MONVER_01_STRING				"1990"
#define  MONVER_02_STRING				"1999"
#define  MONVER_03_STRING				"2005"
#define  MONVER_04_STRING				"2015"


#define  SERVER_CODE_0                   0
#define  SERVER_CODE_1                   1
#define  SERVER_CODE_2                   2
#define  SERVER_CODE_3                   3

typedef unsigned short WORD;  //16位无符号整形
typedef unsigned int DWORD;
typedef unsigned int LONG;   //32位无符号整形

#define  USHORT 					unsigned short			//16位无符号整形
#define  ULONG						unsigned int			//32位无符号整形
//#define  LEFT_ROTATE 				0
#define  RIGHT_ROTATE  				1

#pragma pack(2)
typedef struct tagBITMAPFILEHEADER          // 文件头
{
    WORD bfType;
    LONG bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    LONG bfOffBits;
} BITMAPFILEHEADER;
#pragma pack()
typedef struct tagBITMAPINFOHEADER{         // 信息头
	DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER;

typedef struct t_FileName
{
	char BankNo[BANKNO_LEN+1];			//银行号
	char AgencyNo[AGENCYNO_LEN + 1];	//网点号
	char CounterNo[DESKNO_LEN + 1];		//柜台号/一捆把数
	char BoxNo[BOXNO_LEN+1];			//盒子ID
	char Date[DATE_LEN+1];				//日期
	char PackageId[KUNID_LEN + 1];		//捆ID
	char DateTime[DATE_LEN+TIME_LEN+1];	//日期时间
	char Operator[USERID_LEN + 1]; 		//操作员
	char BusinessType[5];
	char Percode[24+1];
	unsigned int OneRecordSize;
	unsigned int IncludeImageSnoFlag;
	DataType *df;
}FILENAME,*pFileName;

//把捆信息结构
typedef struct t_BundleBobInfo
{
	char BundleId[25];//捆ID
	char BundleDate[20];//打捆时间
	char OperatorId[25];//打捆员
	char Checker[25];//打捆监管员
	char BobId[ONEBUNDLE_BOBNUM][25];//把ID

}BUNDLEBOBINFO,*pBundleBobInfo;


//文件头结构体
typedef struct T_FileHead
{
	USHORT HeadStart[4];	//USHORT:16位无符号整型
	USHORT HeadString[6];
	ULONG  Counter;			//ULONG:32位无符号整型
	USHORT HeadEnd[4];

}FILEHEAD, *pFileHead;

//单个图像号码结构
typedef struct T_ImageSNoData
{
	ULONG Data[32];

}IMAGESNODATA, *pImageSNoData;

//图像冠字号结构
typedef struct T_ImageSNo
{
	USHORT Num;
	USHORT height;
	USHORT width;
	USHORT Reserve2;
	IMAGESNODATA SNo[12];

}IMAGESNO, *pImageSNo;

//文件记录结构
typedef struct t_FileRecord
{
	char Date[10 + 1];		//日期，多+1不然插入报ora01480错
	char Time[8];			//时间
	char tfFlag[1 + 1];		//真假标识
	char ErrorCode[6+1];		//错误码
	char MoneyFlag[4+1];		//币种
	char Ver[5+1];			//版别
	char Valuta[5+1];			//币值
	char CharNUM[2];		//冠字号字符数
	char SNo[12+1];			//冠字号
	char MachineSNo[24 + 1];//机具号
	char Reserve1[50+1];		//保留字
	IMAGESNO ImageSNo;		//冠字号图像
	char ImageSNoChar[2500];
	unsigned char str[10*32*4 + 62 + 1];
	char ImageFilePath[FILE_PATH_CHARNUM];

	//打捆业务
	char BundleID[24 + 1];	//把条码
	char PackageId[KUNID_LEN + 1];	//捆ID
	char AgencyID[8+1];		//网点号
	char Operator[20 + 1];  //操作员
	char Monitor[20 + 1];	//监督员

	//梅州散装加钞业务
	char MonBoxID[24 + 1];		//钞箱ID
	char AtmID[24 + 1];			//ATM机ID
	char AddMonOperator[8 + 1];	//加钞员
	char AddMonChecker[8 + 1];	//加钞监管员
	//int BusinessType;			//业务类型
	char BusinessType[5+1];

	char DateTime[20+1];
	char Quanlity[5+1];
	char BankNo[BANKNO_LEN+1];
	char AgencyNo[AGENCYNO_LEN+1];
	char FileName[FILE_PATH_CHARNUM+1];
	unsigned int iFailInsRecordNum;
}FILERECORD, *pFileRecord;

typedef struct t_FileMoveInfo
{
	char FileBaseName[FILE_NAME_CHARNUM];
	char FilePth[MAX_STRING_SIZE];
	char LocalDesBaseName[FILE_NAME_CHARNUM];
	char LocalDesFilePath[MAX_STRING_SIZE];
	char ReFileBaseName[FILE_NAME_CHARNUM];
	char ReFilePath[MAX_STRING_SIZE];
	char ReBankNO[5];
	char ReIp[20];
	char RePort[10];
    int  isRemote;
    int  isCompress;
}FILEMOVEINFO,*pFileMoveInfo;


typedef struct t_AddMonInfo
{
	char BundleId[24+1];//捆、把ID
	char MachineId[18+1];//ATM编号
	char MonBoxId[11+1];//钞箱ID
	char AddMonOperator[8+1];//加钞员
	char AddMonChecker[8+1];//加钞监管员
	char AddMonDate[19+1];//加钞时间
#ifdef BFNEW
	char Id[30+1];
#endif
}ADDMONINFO,*pAddMonInfo;

int SKFile(DataType *df);
int FSNFile(DataType *df);
int DKFile(DataType *df);
int CTFile(DataType *df);
int BKFile(DataType *df);
int BFFile(DataType *df);
int ATMFile(DataType *df);
int ZIPFile(DataType *df);

int CheckDirEmpty(char *dir);
int ReadFSNOrBKFileName(const char *szFileName, FILENAME *pfilename);
int	WriteToSnoTable(FILENAME *fsnFile);
void CheckDoubtMon(MONEYDATAR *rec);
int CheckFSNRecode(FILERECORD *fileRecord);
void *SearchFileThread(void *pt);
void *SearchPayThread(void *pt);
void *ListDirThread(void *pt);
void *SwitchFileThread(void *pt);
void *HandleFileThread(void *pt);
int CheckFileType(char *type);
void CheckResult(DataType *df,int result);
int StartFile(DataType *df);
int StopFile(DataType *df);
void CoverFiles();
char *GetErrorLocalDir(char *dir,const  FILENAME *fn);
int MoveFiles(FILEMOVEINFO *pInfo);
int CopyFiles(FILEMOVEINFO *pInfo);
char *GetSucInsLocalDir(char *dir,const  FILENAME *fn);
int MoveFSNFile(FILENAME *fn,int result);
void *SendFileThread();
char *GetPercode(const char *src,char *des);
int ReadAtmFileName(const char *szFileName, FILENAME *fn,ATMBUSINESSLIST *business);
int UploadFile(ROUTE *route,ROUTERULE *rule,int ruleSize);
int DownFile(ROUTE *route,ROUTERULE *rule,int ruleSize);
int Start_service(ROUTE *route);
int Stop_service(ROUTE *route);
int SaveSnoImageFromFsnFile(FILENAME *pfilename,FILERECORD *FileRecord,char *list,int iSnoNo);
void *ListTransFormDirThread(void *pt);
#endif
