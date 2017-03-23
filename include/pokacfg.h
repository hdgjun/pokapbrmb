#ifndef _POKACFG_H
#define _POKACFG_H

#include "common.h"

#define  CONFILE_NAME    "pkrmb.ini"

#define  DEF_INSTALL_PATH           "/home/poka/pokapbrmb/"

#define  DEF_DATE_CUT_TIME          "000001"     /*默认00:00:01后日切*/
#define  DEF_DATE_CUT_END           "040000"    /*默认04:00:00结束日切*/
#define  DATE_START                 1
#define  DATE_END                   0

#define  DEF_DB_SAVE_DAY            100               /*数据库中数据保存天数，默认为100天*/
#define  DEF_ONLY_PAY_PB            1                 /*默认仅发送交款调拨文件到人行，仅商行端使用的配置项*/
#define  DEF_LOG_SAVE_DAY           30                /*默认日志保存天数*/
#define  DEF_FILE_SAVE_DAY          90                /*默认文件保存天数*/
#define  DEF_IMAGE_SAVE_DAY         90                /*默认图片保存天数*/
#define  DEF_IMAGE_SAVE             1                  /*默认保存图片*/
#define  DEF_IMAGE_DIR              "image"
#define  DEF_FILE_STORE_BASE_PATH   "/home/poka/"
#define  DEF_SWITCH_DIR             "fsn_put"         /*商行端[文件发送到文件处理目录和转发目录(人行子目录)]人行端[清分中心文件分发到转发目录，其他文件转发到文件处理目录]*/
#define  DEF_HANDLE_DIR             "fsn_cb_handin"   /*[商行端、人行端共同]文件处理后入库目录*/
#define  DEF_INSERT_DIR             "fsn_insert"      /*[商行端、人行端共同]文件处理目录*/
#define  DEF_SEND_FILE_DIR          "fsn_sc_ins"      /*[商行端、人行端共同]转发目录(目的银行号子目录)*/
#define  DEF_ERROR_DIR          	"fsn_error"       /*[商行端、人行端共同]错误文件目录*/
#define  DEF_UPDATE_DIR             "poka_update"     /*[商行端]程序更新目录 */

#define  DEF_TRANSFORM_DIR          "fsn_trans"    /*[商行端、人行端共同]poka fsn 转 标准fsn*/
#define  DEF_TRANSFORM_FIN_DIR      "fsn_trans_fin" /*[商行端、人行端共同]poka fsn 转 标准fsn完成*/
#define  DEF_OPEN_TRANSFORM         0               /*默认0,关闭fsn转化功能*/

/*#define  DEF_PB_LOADING_DIR         "fsn_pb_put"      */
#define  DEF_THREAD_SIZE             10
#define  DEF_SLEEP_TIME              60
#define  DEF_BANKNO                  "0204"

#define  PBNO_KEY                    "pbno"            /*商行必需设置*/
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
#define  LOG_SAVE_DAY_KEY            "logsaveday"
#define  FILE_SAVE_DAY_KEY           "filesaveday"
#define  IMAGE_SAVE_DAY_KEY          "imagesaveday"
#define  TRANSFORM_DIR_KEY           "transformdir"
#define  TRANSFORM_FIN_DIR_KEY       "transformfindir"
#define  OPEN_TRANSFORM__KEY         "opentransform"
#define  ONLY_PAY_PB_KEY             "onlypaytopb"
#define  DB_SAVE_DAY_KEY             "dbsaveday"
//参数结构体
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
	char transformDir[MIN_STRING_SIZE];
	char transformFinDir[MIN_STRING_SIZE];
	int  openTransfom;
	int  LogSaveDay;
	int  FileSaveDay;
	int  ImageSaveDay;
	int  ThreadSize;
	int  SleepTime;
	int  SaveImage;
	int  OnlyPaytoPb;
	int  DbSaveDay;
}Param,*pParam;

Param g_param;


int	ReadParamFromIniFile(Param *param,char *app_home);

#endif
