/*
 * common.h
 *
 *  Created on: 2016��6��20��
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

#define  MAX_STRING_SIZE    		500						//����ַ�������
#define  CUR_STRING_SIZE    		200
#define  MIN_STRING_SIZE			25
#define  MAX_ONE_FSN_SIZE           1000                    //һ��FSN�ļ������ֺ�����
#define  FILE_HEAD_SIZE				32						//�ļ�ͷ��С
#define  STANDARD_ONE_RECORD_SIZE	1644					//��׼FSNһ�����ֺż�¼��С
#define  ONE_RECORD_SIZE			(1644+40+64+1)			//һ�����ֺż�¼��С(40����ID������Ա���ලԱ��64�ǳ���ID��ATMID���ӳ�Ա�����Ա,1�Ǵ�ȡ���־)
#define  ONE_RECORD_IMAGE_SIZE		1544					//һ�����ֺż�¼ͼ���ֶδ�С
#define  ONE_BK_RECORD_SIZE			313						//һ��������¼��С
#define  ONE_BF_RECORD_SIZE			94						//һ���ӳ���¼��С
#define  ONE_CT_RECORD_SIZE			54						//һ��ȡ���¼��С

#define  ONEBUNDLE_BOBNUM			10						//һ����������
#define  ONEBAG_KUNNUM				20						//һ����������

#define  FILE_PATH_CHARNUM			256
#define	 FILE_NAME_CHARNUM			255
#define  PARAM_NUM                  100


#define  CONFFILE_BUF_SIZE          256

#define	 SEPARATOR_STRING			"_"					//�ָ���
#define	 ATM_SEPARATOR_STRING		"#"					//�ָ���
#define  TAB_CHAR_STRING			"\t"					//tab�ַ�
#define  ENTER_CHAR_STRING			"\n"					//�����ַ�
#define  COMMA_CHAR_STRING			","						//Ӣ�Ķ����ַ�
#define	 RUNG_CHAR_STRING			"-"						//����ַ�
#define	 COLON_CHAR_STRING			":"						//ð���ַ�
#define	 SPACE_CHAR_STRING			" "						//�ո��ַ�


#define	 ONLYREAD_ACCESS_STRING		"r"						//���ļ�ֻ��Ȩ��
#define	 ONLYCREATE_ACCESS_STRING	"w"						//���ļ�ֻдȨ��
#define	 ADD_ACCESS_STRING			"a+"					//���ļ�׷��Ȩ��
#define	 CREATEBYTE_ACCESS_STRING	"wb"					//���ļ�������дȨ��
#define	 READBYTE_ACCESS_STRING		"rb"					//���ļ������ƶ�Ȩ��
#define	 ADDBYTE_ACCESS_STRING		"ab+"					//���ļ�������׷��Ȩ��

#define	 FTP_GETSHELLNAME_STRING	 "getfile_ftp.sh"			//ftp��ȡ�ļ�shell�ű�����
#define	 SFTP_GETSHELLNAME_STRING 	 "getfile_sftp.sh"			//sftp��ȡ�ļ�shell�ű�����
#define	 FTP_MOVESHELLNAME_STRING	 "movefile_ftp.sh"			//ftp�ƶ��ļ�shell�ű�����
#define	 SFTP_MOVESHELLNAME_STRING	 "movefile_sftp.sh"			//sftp�ƶ��ļ�shell�ű�����
#define	 FTP_PUTSHELLNAME_STRING	 "putfile_ftp.sh"			//ftp�����ļ�shell�ű�����
#define	 SFTP_PUTSHELLNAME_STRING 	 "putfile_sftp.sh"			//sftp�����ļ�shell�ű�����
#define  FTP_GETDIRSHELLNAME_STRING  "getdir_ftp.sh"
#define  SFTP_GETDIRSHELLNAME_STRING "getdir_sftp.sh"
#define  DELETE_IMAGE_SHELLNAME_STRING "deleteimage.sh"
#define  DELETE_FSN_SHELLNAME_STRING	"deletefsn.sh"
#define	 ZIPCMD_STRING				"zip"					//zipѹ�������ַ���

#define  TIMEFILL_STRING			"XXXXXX"

#define  BANKNO_LEN 					4//���к�λ��
#define  AGENCYNO_LEN 					4//�����λ��
#define  BOXNO_LEN 						14//���Ӻ�λ��
#define  BAID_LEN 						24//��IDλ��
#define  KUNID_LEN 						24//��IDλ��
#define  DAIID_LEN 						20//��IDλ��
#define  ATMID_LEN 						18//ATM��IDλ��
#define  MONBOXID_LEN 					11//����IDλ��

#define  SNO_LEN 						12//���ֺ�λ��
#define  PERCODE_LEN 					24//���ߺ�λ��
#define  USERID_LEN 					8//�û�IDλ��
#define  DESKNO_LEN 					2//��̨��λ��
#define  BANKSEQ_LEN 					24//������ˮ��λ��
#define  DATE_LEN						8//����λ��
#define  TIME_LEN						8//ʱ��λ��
#define  BUSINESSTYPE_LEN				1//ҵ������λ��

#define	 ONE_YEAR_MONTHS				12//һ��12����
#define	 MAX_ONE_MONTH_DAYS				31//һ���������
#define	 ONE_DAY_HOURS					24//һ��24��Сʱ
#define	 ONE_HOUR_MINUTES				60//һСʱ60����
#define	 ONE_MINUTE_SECONDS				60//һ����60��
#define	 ONE_SECOND_MILLISECONDS		1000//һ��1000����

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
