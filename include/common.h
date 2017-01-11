/*
 * common.h
 *
 *  Created on: 2016年6月20日
 *      Author: Administrator
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <pthread.h>

#define SUCESS 	0
#define WARING 	1
#define ERROR 	-1
#define NODATA 	-2
#define S_START  '0'
#define S_STOP   '1'

#define  MAX_THREAD_NUM    			50
#define  MAX_SLEEP_TIME    			60*10

#define  MAX_STRING_SIZE    		500						//最大字符串长度
#define  CUR_STRING_SIZE    		200
#define  MIN_STRING_SIZE			25
#define  MAX_ONE_FSN_SIZE           1000                    //一个FSN文件最大冠字号数量
#define  FILE_HEAD_SIZE				32						//文件头大小
#define  STANDARD_ONE_RECORD_SIZE	1644					//标准FSN一条冠字号记录大小
#define  ONE_RECORD_SIZE			(1644+40+64+1)			//一条冠字号记录大小(40是捆ID、操作员、监督员，64是钞箱ID、ATMID、加钞员、监管员,1是存取款标志)
#define  ONE_RECORD_IMAGE_SIZE		1544					//一条冠字号记录图像字段大小
#define  ONE_BK_RECORD_SIZE			313						//一条把捆记录大小
#define  ONE_BF_RECORD_SIZE			94						//一条加钞记录大小
#define  ONE_CT_RECORD_SIZE			54						//一条取款记录大小

#define  ONEBUNDLE_BOBNUM			10						//一捆包含把数
#define  ONEBAG_KUNNUM				20						//一袋包含捆数

#define  FILE_PATH_CHARNUM			256
#define	 FILE_NAME_CHARNUM			255
#define  PARAM_NUM                  100


#define  CONFFILE_BUF_SIZE          256

#define	 SEPARATOR_STRING			"_"					//分隔符
#define	 ATM_SEPARATOR_STRING		"#"					//分隔符
#define  TAB_CHAR_STRING			"\t"					//tab字符
#define  ENTER_CHAR_STRING			"\n"					//换行字符
#define  COMMA_CHAR_STRING			","						//英文逗号字符
#define	 RUNG_CHAR_STRING			"-"						//横杠字符
#define	 COLON_CHAR_STRING			":"						//冒号字符
#define	 SPACE_CHAR_STRING			" "						//空格字符


#define	 ONLYREAD_ACCESS_STRING		"r"						//打开文件只读权限
#define	 ONLYCREATE_ACCESS_STRING	"w"						//打开文件只写权限
#define	 ADD_ACCESS_STRING			"a+"					//打开文件追加权限
#define	 CREATEBYTE_ACCESS_STRING	"wb"					//打开文件二进制写权限
#define	 READBYTE_ACCESS_STRING		"rb"					//打开文件二进制读权限
#define	 ADDBYTE_ACCESS_STRING		"ab+"					//打开文件二进制追加权限

#define	 FTP_GETSHELLNAME_STRING	 "getfile_ftp.sh"			//ftp获取文件shell脚本名称
#define	 SFTP_GETSHELLNAME_STRING 	 "getfile_sftp.sh"			//sftp获取文件shell脚本名称
#define	 FTP_MOVESHELLNAME_STRING	 "movefile_ftp.sh"			//ftp移动文件shell脚本名称
#define	 SFTP_MOVESHELLNAME_STRING	 "movefile_sftp.sh"			//sftp移动文件shell脚本名称
#define	 FTP_PUTSHELLNAME_STRING	 "putfile_ftp.sh"			//ftp发送文件shell脚本名称
#define	 SFTP_PUTSHELLNAME_STRING 	 "putfile_sftp.sh"			//sftp发送文件shell脚本名称
#define  FTP_GETDIRSHELLNAME_STRING  "getdir_ftp.sh"
#define  SFTP_GETDIRSHELLNAME_STRING "getdir_sftp.sh"
#define  DELETE_IMAGE_SHELLNAME_STRING "deleteimage.sh"
#define  DELETE_FSN_SHELLNAME_STRING	"deletefsn.sh"
#define	 ZIPCMD_STRING				"zip"					//zip压缩命令字符串

#define  TIMEFILL_STRING			"XXXXXX"

#define  BANKNO_LEN 					4//银行号位数
#define  AGENCYNO_LEN 					4//网点号位数
#define  BOXNO_LEN 						14//盒子号位数
#define  BAID_LEN 						24//把ID位数
#define  KUNID_LEN 						24//捆ID位数
#define  DAIID_LEN 						20//袋ID位数
#define  ATMID_LEN 						18//ATM机ID位数
#define  MONBOXID_LEN 					11//钞箱ID位数

#define  SNO_LEN 						12//冠字号位数
#define  PERCODE_LEN 					24//机具号位数
#define  USERID_LEN 					8//用户ID位数
#define  DESKNO_LEN 					2//柜台号位数
#define  BANKSEQ_LEN 					24//银行流水号位数
#define  DATE_LEN						8//日期位数
#define  TIME_LEN						8//时间位数
#define  BUSINESSTYPE_LEN				1//业务类型位数

#define	 ONE_YEAR_MONTHS				12//一年12个月
#define	 MAX_ONE_MONTH_DAYS				31//一月最多天数
#define	 ONE_DAY_HOURS					24//一天24个小时
#define	 ONE_HOUR_MINUTES				60//一小时60分钟
#define	 ONE_MINUTE_SECONDS				60//一分钟60秒
#define	 ONE_SECOND_MILLISECONDS		1000//一秒1000毫秒

pthread_key_t p_Thread_key;

typedef struct WAITEFORTIME
{
 	pthread_mutex_t mutex;
 	pthread_cond_t cond;
 	pthread_condattr_t conattr;
}waitfortime;


typedef enum FTP_MODE
{
	FTP_MPUT,
	FTP_MGET,
	FTP_PUT,
	FTP_GET
}FTP_MODE;

typedef struct FTP_MESSAGE
{
	char ip[20+1];
	char port[20+1];
	char user[20+1];
	char passwd[20+1];
	char remotedir[200+1];
	char localdir[200+1];
	char filename[200+1];
	char logname[200+1];
	FTP_MODE model;
}FTP_MESSAGE;

int JudgeProgramExist(int flag,const char *appName);
int test_pthread(pthread_t tid);
void psleep(int timeOut);
char *GetProgramPath(char *path,const char *enName,const char *def);

#endif /* COMMON_H_ */
