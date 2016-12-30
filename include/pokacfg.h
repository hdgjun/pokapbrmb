#ifndef _POKACFG_H
#define _POKACFG_H

#include "common.h"

#define  CONFILE_NAME    "pkrmb.ini"

#define  DEF_INSTALL_PATH           "/home/poka/pokapbrmb/"

#define  DEF_DATE_CUT_TIME          "000001"     /*Ĭ��00:00:01������*/
#define  DEF_DATE_CUT_END           "040000"    /*Ĭ��04:00:00��������*/
#define  DATE_START                 1
#define  DATE_END                   0

#define  DEF_IMAGE_SAVE             1                  /*Ĭ�ϱ���ͼƬ*/
#define  DEF_IMAGE_DIR              "image"
#define  DEF_FILE_STORE_BASE_PATH   "/home/poka/"
#define  DEF_SWITCH_DIR             "fsn_put"         /*���ж�[�ļ����͵��ļ�����Ŀ¼��ת��Ŀ¼(������Ŀ¼)]���ж�[��������ļ��ַ���ת��Ŀ¼�������ļ�ת�����ļ�����Ŀ¼]*/
#define  DEF_HANDLE_DIR             "fsn_cb_handin"   /*[���жˡ����ж˹�ͬ]�ļ����������Ŀ¼*/
#define  DEF_INSERT_DIR             "fsn_insert"      /*[���жˡ����ж˹�ͬ]�ļ�����Ŀ¼*/
#define  DEF_SEND_FILE_DIR          "fsn_sc_ins"      /*[���жˡ����ж˹�ͬ]ת��Ŀ¼(Ŀ�����к���Ŀ¼)*/
#define  DEF_ERROR_DIR          	"fsn_error"       /*[���жˡ����ж˹�ͬ]�����ļ�Ŀ¼*/
#define  DEF_UPDATE_DIR             "poka_update"     /*[���ж�]�������Ŀ¼ */
/*#define  DEF_PB_LOADING_DIR         "fsn_pb_put"      */
#define  DEF_THREAD_SIZE             10
#define  DEF_SLEEP_TIME              60
#define  DEF_BANKNO                  "0204"

#define  PBNO_KEY                    "pbno"            /*���б�������*/
#define  BANKNO_KEY                  "bankno"
#define  DBUSER_KEY                  "dbuser"
#define  DBPASSWORD_KEY              "dbpassword"
#define  DBSERVICENAME_KEY           "dbservicename"
#define  DBIP_KEY                    "dbip"
#define  DBPORT_KEY                  "dbport"
#define  FILESTROEBASEPATH_KEY       "filestorebasepath"
#define  SWITCHDIR_KEY               "switchdir"
#define  HANDLEDIR_KEY               "handledir"
#define  INSERTDIR_KEY               "insertdir"
#define  SENDFILEDIR_KEY             "sendfiledir"
#define  UPDATEDIR_KEY               "updatedir"
#define  ERRORDIR_KEY                "errordir"
/*#define  PBLOADINGDIR_KEY            "pbloadingdir"*/
#define  THREADSIZE_KEY              "threadsize"
#define  SLEEPTIME_KEY               "sleeptime"
#define  IMAGE_PATH_KEY              "imagepath"
#define  IMAGE_DIR_KEY               "imagedir"
#define  IMAGE_SAVE_KEY              "saveimage"

//�����ṹ��
typedef struct t_Param
{
	char BankNo[BANKNO_LEN+1];
	char PBno[BANKNO_LEN+1];
	char DBUser[MIN_STRING_SIZE];
	char DBPassword[MIN_STRING_SIZE];
	char DBServiceName[MIN_STRING_SIZE];
	char DBIP[MIN_STRING_SIZE];
	char DBPort[MIN_STRING_SIZE];
	char FileStoreBasePath[MAX_STRING_SIZE];
	char SwitchDir[MIN_STRING_SIZE];
	char handleDir[MIN_STRING_SIZE];
	char InsertDir[MIN_STRING_SIZE];
	char SendFileDIr[MIN_STRING_SIZE];
	char UpdateDIr[MIN_STRING_SIZE];
	char ErrorDir[MIN_STRING_SIZE];
	char ImagePath[MIN_STRING_SIZE];
	char ImageDir[MIN_STRING_SIZE];
/*	char PbLoadingDir[MIN_STRING_SIZE];*/
	int  ThreadSize;
	int  SleepTime;
	int  SaveImage;
}Param,*pParam;

Param g_param;


int	ReadParamFromIniFile(Param *param,char *app_home);

#endif