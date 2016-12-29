#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "pokalog.h"
#include "pokaftp.h"

int main()
{
	FTP_MESSAGE msg;
	memset(&msg,0x00,sizeof(FTP_MESSAGE));
	sprintf(msg.ip,"192.168.125.112");
	sprintf(msg.port,"10001");
	sprintf(msg.user,"poka");
	sprintf(msg.localdir,"/home/poka/hdgjun/");
	sprintf(msg.remotedir,"/");
	sprintf(msg.logname,"ftp.log");
	sprintf(msg.filename,"*");
	msg.model = FTP_MGET;

	shellftp(&msg);
	return 1;
}
