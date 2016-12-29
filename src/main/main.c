#include <stdio.h>
#include "pokaftp.h"
int main(){

	curl_init();
	FTP_OPT ftp_opt = {0};

	Get_download_ftp_opt(&ftp_opt,"192.168.125.126","21"
				,"","5.txt","/home/poka/pokapbrmb/","123456.txt"
				,"poka","123456");

	if(FTP_DOWNLOAD_SUCCESS == ftp_download(&ftp_opt))
		printf("Download success.\n");
	else
		printf("Download failed.\n");

	Get_upload_ftp_opt(&ftp_opt,"192.168.125.126","21"
			,"/home/poka/pokapbrmb/","makefile","/fsn/","_makefile_1234.mk"
			,"poka","123456");

	if(FTP_UPLOAD_SUCCESS == ftp_upload(&ftp_opt))
		printf("UPLOAD success.\n");
	else
		printf("UPLOAD failed.\n");
	clear_curl((void *)0);

	return 0;
}
