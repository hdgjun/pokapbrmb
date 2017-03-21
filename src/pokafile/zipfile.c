#include <stdio.h>
#include <stdlib.h>
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
	DataType file;
	memset(&file,0x00,sizeof(DataType));
	memcpy(file.filePath,df->filePath,strlen(df->filePath));
	sprintf(file.fileName,"%s.%s",df->fileName,START_FILE_STRING);
	return unZip(&file);
}
