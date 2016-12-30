#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "pokafile.h"
#include "common.h"
#include "filepath.h"

#define  INCLUDEIMAGE_FLAG_STRING		"0x2E"
#define  NOTINCLUDEIMAGE_FLAG_STRING	"0x2D"

int RotateImage(FILENAME *pfilename,FILERECORD *FileRecord/*unsigned char str[10*32*4 + 62 + 1]*/);

static unsigned char HEAD[62]={
	0x42,0x4d,//file type
	0x3E,0x05,0x00,0x00, //file size***
	0x00,0x00, //reserved
	0x00,0x00,//reserved
	0x3E,0x00,0x00,0x00,//head byte***
	0x28,0x00,0x00,0x00,//struct size
	0x20,0x00,0x0,0x00,//map width***
	0x40,0x01,0x00,0x00,//map height***
	0x01,0x00,//must be 1
	0x01,0x00,//color count***
	0x00,0x00,0x00,0x00, //compression
	0x00,0x00,0x05,0x00,//data size***
	//0x00,0x00,0x00,0x00,//data size***
	0x00,0x00,0x00,0x00, //dpix
	0x00,0x00,0x00,0x00, //dpiy
	0x00,0x00,0x00,0x00,//color used
	0x00,0x00,0x00,0x00,//color important
	0xFF,0xFF,0xFF,0x00,
	0x00,0x00,0x00,0x00,
};

//截取图片
int SaveSnoImageFromFsnFile(FILENAME *pfilename,FILERECORD *FileRecord,char *list,int iSnoNo)
{
	if(list == NULL)
	{
		printf("SaveSnoImageFromFsnFile Fsn file read buf is null\n");
		vLog("SaveSnoImageFromFsnFile Fsn file read buf is null");
		return ERROR;
	}

	//冠字号图像数据
	memcpy(&FileRecord->ImageSNo, &list[iSnoNo*pfilename->OneRecordSize+132], sizeof(FileRecord->ImageSNo));

	unsigned int n, k;
	ULONG temp[32] = {0};
	ULONG int_a = 0;
	ULONG temp_data[32] = {0};
	unsigned char str[10*32*4 + 62 + 1] = {0};

	memcpy(str, HEAD, 62);
	for (n = 0; n < 10; n++)
	{
		memset(temp, 0, sizeof(unsigned int)*32);
		memset(temp_data, 0, sizeof(unsigned int)*32);
		memcpy(temp, FileRecord->ImageSNo.SNo[n].Data, 32*4);
		for (k = 0; k < 32; k++)
		{
			temp_data[k] += temp[k] & 0xFF;
			temp_data[k] <<= 24;
			temp_data[k] += (temp[k] << 8)& 0x00FF0000;
			temp_data[k] += (temp[k] >> 8)& 0xFF00;
			temp_data[k] += (temp[k] >> 24)& 0xFF;
		}
		memcpy(str + 62 + n*32*4, temp_data, 32*4);
	}
	memcpy(FileRecord->str, str, 10*32*4 + 62);

	//旋转90度
	RotateImage(pfilename,FileRecord);

	return SUCESS;
}

int RotateImage(FILENAME *pfilename,FILERECORD *FileRecord/*unsigned char str[10*32*4 + 62 + 1]*/)
{
	unsigned long ulStrSize = 10*32*4 + 62 + 1;
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    unsigned char palette[8];
    unsigned char str[10*32*4 + 62 + 1] = {0};

	memcpy(&str,FileRecord->str,10*32*4 + 62 +1);

	char strNumSNo[MAX_STRING_SIZE] = {0};
	memset(strNumSNo,0,sizeof(strNumSNo));
	sprintf(strNumSNo,"0x%x%x",(unsigned char)(str[1]),(unsigned char)(str[0]));
	fileHeader.bfType = htoi1(strNumSNo);
	sprintf(strNumSNo,"0x%x%x%x%x",(unsigned char)(str[5]),(unsigned char)(str[4]),(unsigned char)(str[3]),(unsigned char)(str[2]));
	fileHeader.bfSize = htoi1(strNumSNo);
	sprintf(strNumSNo,"0x%x%x",(unsigned char)(str[7]),(unsigned char)(str[6]));
	fileHeader.bfReserved1 = htoi1(strNumSNo);
	sprintf(strNumSNo,"0x%x%x",(unsigned char)(str[9]),(unsigned char)(str[8]));
	fileHeader.bfReserved2 = htoi1(strNumSNo);
	sprintf(strNumSNo,"0x%x%x%x%x",(unsigned char)(str[13]),(unsigned char)(str[12]),(unsigned char)(str[11]),(unsigned char)(str[10]));
	fileHeader.bfOffBits = htoi1(strNumSNo);

	sprintf(strNumSNo,"0x%x%x%x%x",(unsigned char)(str[17]),(unsigned char)(str[16]),(unsigned char)(str[15]),(unsigned char)(str[14]));
	infoHeader.biSize = htoi1(strNumSNo);
	sprintf(strNumSNo,"0x%x%x%x%x",(unsigned char)(str[21]),(unsigned char)(str[20]),(unsigned char)(str[19]),(unsigned char)(str[18]));
	infoHeader.biWidth = htoi1(strNumSNo);
	sprintf(strNumSNo,"0x%x%x%x%x",(unsigned char)(str[25]),(unsigned char)(str[24]),(unsigned char)(str[23]),(unsigned char)(str[22]));
	infoHeader.biHeight = htoi1(strNumSNo);
	sprintf(strNumSNo,"0x%x%x",(unsigned char)(str[27]),(unsigned char)(str[26]));
	infoHeader.biPlanes = htoi1(strNumSNo);
	sprintf(strNumSNo,"0x%x%x",(unsigned char)(str[29]),(unsigned char)(str[28]));
	infoHeader.biBitCount = htoi1(strNumSNo);
	sprintf(strNumSNo,"0x%x%x%x%x",(unsigned char)(str[33]),(unsigned char)(str[32]),(unsigned char)(str[31]),(unsigned char)(str[30]));
	infoHeader.biCompression = htoi1(strNumSNo);
	sprintf(strNumSNo,"0x%x%x%x%x",(unsigned char)(str[37]),(unsigned char)(str[36]),(unsigned char)(str[35]),(unsigned char)(str[34]));
	infoHeader.biSizeImage = htoi1(strNumSNo);
	sprintf(strNumSNo,"0x%x%x%x%x",(unsigned char)(str[41]),(unsigned char)(str[40]),(unsigned char)(str[39]),(unsigned char)(str[38]));
	infoHeader.biXPelsPerMeter = htoi1(strNumSNo);
	sprintf(strNumSNo,"0x%x%x%x%x",(unsigned char)(str[45]),(unsigned char)(str[44]),(unsigned char)(str[43]),(unsigned char)(str[42]));
	infoHeader.biYPelsPerMeter = htoi1(strNumSNo);
	sprintf(strNumSNo,"0x%x%x%x%x",(unsigned char)(str[49]),(unsigned char)(str[48]),(unsigned char)(str[47]),(unsigned char)(str[46]));
	infoHeader.biClrUsed = htoi1(strNumSNo);
	sprintf(strNumSNo,"0x%x%x%x%x",(unsigned char)(str[53]),(unsigned char)(str[52]),(unsigned char)(str[51]),(unsigned char)(str[50]));
	infoHeader.biClrImportant = htoi1(strNumSNo);

	unsigned int i = 0;
	for(i = 0; i < 8; i++)
	{
		palette[i] = str[54 + i];
	}

    unsigned char *data = (unsigned char*)malloc(infoHeader.biSizeImage);
	for(i = 0; i < infoHeader.biSizeImage; i++)
	{
		data[i] = str[62 + i];
	}

	FILE *pImageFile = NULL;
	pImageFile = fopen(FileRecord->ImageFilePath,CREATEBYTE_ACCESS_STRING);
	if(pImageFile == NULL)
	{
		printf("Save image file fail:%s\n",strerror(errno));
		vLog("Save image file fail:%s",strerror(errno));
		return ERROR;
	}
	else
	{
		BITMAPFILEHEADER oFileHeader;
	    BITMAPINFOHEADER oInfoHeader;

	    int lineSize = ( infoHeader.biHeight + 31) / 32 * 4;
	    int imageSize = lineSize * infoHeader.biWidth;

	    oFileHeader.bfType = 19778;
	    oFileHeader.bfSize = 62 + imageSize;
	    oFileHeader.bfReserved1 = 0;
	    oFileHeader.bfReserved2 = 0;
	    oFileHeader.bfOffBits = 62;

	    oInfoHeader.biSize = 40;
	    oInfoHeader.biWidth = infoHeader.biHeight;
	    oInfoHeader.biHeight = infoHeader.biWidth;
	    oInfoHeader.biPlanes = 1;
	    oInfoHeader.biBitCount = 1;
	    oInfoHeader.biCompression = 0;
	    oInfoHeader.biSizeImage = imageSize;
	    oInfoHeader.biXPelsPerMeter = 0;
	    oInfoHeader.biYPelsPerMeter = 0;
	    oInfoHeader.biClrUsed = 0;
	    oInfoHeader.biClrImportant = 0;

	    fwrite( &oFileHeader , sizeof(BITMAPFILEHEADER) , 1 , pImageFile );     // 写入文件头
	    fwrite( &oInfoHeader , sizeof(BITMAPINFOHEADER) , 1 , pImageFile );     // 写入信息头
	    fwrite( palette , 1 , 8 , pImageFile );                                 // 写入调色板

	    int myline = oInfoHeader.biSizeImage / oInfoHeader.biHeight;

	    unsigned char *first = (unsigned char*)malloc(infoHeader.biHeight * oInfoHeader.biHeight);
	    unsigned char *second = (unsigned char*)malloc(infoHeader.biHeight * oInfoHeader.biHeight);
	    unsigned char *third = (unsigned char*)malloc(myline * 8 * oInfoHeader.biHeight);
	    unsigned char *forth = (unsigned char*)malloc(myline * oInfoHeader.biHeight);


	    // first step
	    int j,k,t = -1;
	    int temp = infoHeader.biSizeImage / infoHeader.biHeight;
	    for(i = 0; i < infoHeader.biHeight; i++) {
	        for(j = 0; j < temp; j++) {
	            for(k = 0; k < 8; k++) {
	                if(j*8 + k < oInfoHeader.biHeight) {
	                    first[++t] = data[i * temp + j] >> (8-1-k) & 1;
	                }
	            }
	        }
	    }

		int	cw = RIGHT_ROTATE;
	    // second step
	    // rotate
	    t = -1;
	    for(i = 0; i < oInfoHeader.biHeight; i++) {
	        for(j = 0; j < infoHeader.biHeight; j++) {
	            if(cw == RIGHT_ROTATE) {
	                second[++t] = first[oInfoHeader.biHeight*j + oInfoHeader.biHeight - i - 1];
	            } else {
	                second[++t] = first[oInfoHeader.biHeight*(infoHeader.biHeight - j - 1) + i];
	            }
	        }
	    }

	    // step three
	    // fill with 0's
	    t = -1;
	    for(i = 0; i < oInfoHeader.biHeight; i++) {
	        for(j = 0; j < infoHeader.biHeight; j++) {
	            third[++t] = second[i*infoHeader.biHeight + j];
	        }
	        for(k = infoHeader.biHeight; k < myline * 8; k++) {
	            third[++t] = 0;
	        }
	    }

	    // step four
	    // transfer
	    int u = -1;
	    int m = -1;
	    for(i = 0; i < oInfoHeader.biHeight * myline; i++) {
	            int sum = 0;
	            for(k = 0; k < 8; k++) {
	                sum += npow(2,8-k-1)*third[++m];
	            }
	            forth[++u] = sum;
	    }

		unsigned int uiwrite = 0;
	    uiwrite = fwrite( forth , 1 , myline * oInfoHeader.biHeight , pImageFile );       //  写入数据部分
#if 0
		printf("uiwrite:%d\n",uiwrite);
#endif
		fclose(pImageFile);
		pImageFile = NULL;
	    // 释放内存
		free(data);
	    data = NULL;
	    free(forth);
	    forth = NULL;
	    free(third);
	    third = NULL;
	    free(second);
	    second = NULL;
	    free(first);
	    first = NULL;
	}

	return SUCESS;
}


int JudgeIncludeImageSno(char *szReadBuf)
{
	if (!szReadBuf||strcmp(szReadBuf,"") == 0)//判断buf是否为空
	{
		printf("JudgeIncludeImageSno szReadBuf is null!\n");
		vLog("JudgeIncludeImageSno szReadBuf is null!");
		return ERROR;
	}
	char strImageFlag[MAX_STRING_SIZE] = {0};
	memset(strImageFlag,0,sizeof(strImageFlag));
	//是否包含图像标志
	sprintf(strImageFlag,"0x%02x",(unsigned char)(szReadBuf[12]));

	if(0 == strcasecmp(strImageFlag,INCLUDEIMAGE_FLAG_STRING))
	{
		printf("JudgeIncludeImageSno strImageFlag:%s INCLUDEIMAGE_FLAG_STRING:%s\n",strImageFlag,INCLUDEIMAGE_FLAG_STRING);
		return SUCESS;
	}
	return ERROR;
}


