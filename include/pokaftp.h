#ifndef _POAK_FTP_H_
#define _POAK_FTP_H_
#include <curl/curl.h>

#define  FTP_RNFR   "RNFR"
#define  FTP_RNTO   "RNTO"

typedef enum FTP_STATE
{
	FTP_UPLOAD_SUCCESS,
	FTP_UPLOAD_FAILED,
	FTP_DOWNLOAD_SUCCESS,
	FTP_DOWNLOAD_FAILED
}FTP_STATE;

/*FTP OPERATIONS OPTIONS*/
typedef struct FTP_OPT
{
	char url[100];		/*url of ftp*/
	char user_key[50];		/*username:password*/
	char local_file_path[100];		/*filepath*/
	char tmp_name[30];
	char save_name[30];
	char local_name[30];
}FTP_OPT;


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

#ifdef __cplusplus
	extern "C" {
#endif

void curl_init();
void clear_curl(CURL *curl);
FTP_OPT *Get_upload_ftp_opt(FTP_OPT *opt,char *ip,char *port
		,char *localpath,char *localfile,char *savepath,char *savename
		,char *user,char *password);

FTP_OPT *Get_download_ftp_opt(FTP_OPT *opt,char *ip,char *port
		,char *remotepath,char *remotefile,char *savepath,char *savename
		,char *user,char *password);

/*upload file to ftp server*/
FTP_STATE ftp_upload(const FTP_OPT *ftp_option);

/*download file from ftp server*/
FTP_STATE ftp_download(const FTP_OPT *ftp_option);



int shellftp1(FTP_MESSAGE *msg);

int Openftp(FILE *fp,FTP_MESSAGE *msg);
int Sendftp(FILE *fp,FTP_MESSAGE *msg);
int Closeftp(FILE *fp,FTP_MESSAGE *msg);

#ifdef __cplusplus
	}
#endif
#endif
