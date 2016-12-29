#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pokaftp.h"
#include "switch.h"

struct FtpFile {
  const char *filename;
  FILE *stream;
};


static size_t
write_response(void *ptr, size_t size, size_t nmemb, void *data)
{
  FILE *writehere = (FILE *)data;
  return fwrite(ptr, size, nmemb, writehere);
}

static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
  struct FtpFile *out=(struct FtpFile *)stream;
  if(out && !out->stream) {
    /* open file for writing */
    out->stream=fopen(out->filename, "wb");
    if(!out->stream)
      return -1; /* failure, can't open file to write */
  }
  return fwrite(buffer, size, nmemb, out->stream);
}

int get_file_size(FILE *file)
{
	int size = 0;
	fseek(file, 0L, SEEK_END);
	size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	return size;
}
void curl_init()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);

}
CURL *Get_curl()
{
		CURL *curl = curl_easy_init();
		if(NULL == curl)
		{
			fprintf(stderr, "Init curl failed.\n");
			return NULL;
		}
		return curl;
}
struct curl_slist *GetSils(FTP_OPT *opt,struct curl_slist *headerlist)
{
	char buf_1 [100] = {0};
	char buf_2 [100] = {0};
	char temName[50] = {0};
	char remoteName[50] ={0};

	memcpy(temName,opt->tmp_name,strlen(opt->tmp_name));

	memcpy(remoteName,opt->save_name,strlen(opt->save_name));

	printf("temName:%s local_name%s \n",temName,remoteName);
	sprintf(buf_1,"%s %s",FTP_RNFR,temName);
	sprintf(buf_2,"%s %s",FTP_RNTO,remoteName);

	headerlist = curl_slist_append(headerlist, buf_1);
	headerlist = curl_slist_append(headerlist, buf_2);
	return headerlist;
}
void curl_set_upload_opt(CURL *curl, const FTP_OPT *opt, FILE *file,struct curl_slist *headerlist)
{

	GetSils(opt,headerlist);
	curl_easy_setopt(curl, CURLOPT_URL, opt->url);
	curl_easy_setopt(curl, CURLOPT_USERPWD, opt->user_key);
	curl_easy_setopt(curl, CURLOPT_READDATA, file);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
	curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);
	curl_easy_setopt(curl, CURLOPT_INFILESIZE, get_file_size(file));
	curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1);
	curl_easy_setopt(curl,CURLOPT_NOSIGNAL,1L);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
}

void curl_set_download_opt(CURL *curl, const FTP_OPT *opt, struct FtpFile *file,struct curl_slist *headerlist)
{

	curl_easy_setopt(curl, CURLOPT_URL, opt->url);
	curl_easy_setopt(curl, CURLOPT_USERPWD, opt->user_key);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
	//curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL,1L);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
}



void curl_exit(CURL *curl)
{
	if(curl)
	curl_easy_cleanup(curl);
}

void clear_curl(CURL *curl)
{
	if(curl)
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}
CURLcode curl_perform(CURL *curl)
{
	CURLcode ret = curl_easy_perform(curl);
	if(ret != 0)
	{
		fprintf(stderr, "Perform curl failed: %s\n",
		              curl_easy_strerror(ret));
		//curl_exit(curl);
	}
	return ret;
}

/****************ftp upload & download api******************/
FTP_STATE ftp_upload(const FTP_OPT *ftp_option)
{
	FTP_STATE state;
	CURL *curl;
	struct curl_slist *headerlist = NULL;

	char local_tem_path[100] = {0};

	sprintf(local_tem_path,"%s/%s",ftp_option->local_file_path,ftp_option->local_name);

	FILE *fp = fopen(local_tem_path, "r");
	if(NULL == fp)
	{
		fprintf(stderr, "Open file failed at %s:%d filename:%s\n", __FILE__, __LINE__,local_tem_path);
		return FTP_UPLOAD_FAILED;
	}

	curl = Get_curl();
	if(!curl){
		return FTP_UPLOAD_FAILED;
	}
	curl_set_upload_opt(curl, ftp_option, fp,headerlist);
	if(CURLE_OK == curl_perform(curl))
		state = FTP_UPLOAD_SUCCESS;
	else
		state = FTP_UPLOAD_FAILED;

	curl_slist_free_all(headerlist);
	curl_exit(curl);
	fclose(fp);

	return state;
}

FTP_STATE ftp_download(const FTP_OPT *ftp_option)
{
	FTP_STATE state;
	CURL *curl;
	struct curl_slist *headerlist=NULL;

	char local_tem_path[100] = {0};
	char local_save_path[100] = {0};

	sprintf(local_tem_path,"%s/%s",ftp_option->local_file_path,ftp_option->tmp_name);
	sprintf(local_save_path,"%s/%s",ftp_option->local_file_path,ftp_option->save_name);

	struct FtpFile fp = {0};
	fp.filename =&local_tem_path;
	fp.stream=NULL;
#ifdef DEBUG
	FILE *respfile = fopen("/home/poka/pokapbrmb/respfile", "wb");
	if(NULL == respfile)
	{
		fprintf(stderr, "Open file failed at %s:%d filename:%s\n", __FILE__, __LINE__,local_tem_path);
		return FTP_UPLOAD_FAILED;
	}
#endif
	curl = Get_curl();
	curl_set_download_opt(curl, ftp_option, &fp,headerlist);
#ifdef DEBUG
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_response);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, respfile);
#endif
	if(CURLE_OK == curl_perform(curl))
		state = FTP_DOWNLOAD_SUCCESS;
	else
		state = FTP_DOWNLOAD_FAILED;

	curl_slist_free_all(headerlist);
	curl_exit(curl);
#ifdef DEBUG
	fclose(respfile);
#endif
	if(fp.stream)
	    fclose(fp.stream);

	if(state == FTP_DOWNLOAD_SUCCESS){
		rename(local_tem_path, local_save_path);
	}else{
		remove(local_tem_path);
	}
	return state;
}

FTP_OPT *Get_upload_ftp_opt(FTP_OPT *opt,char *ip,char *port
		,char *localpath,char *localfile,char *savepath,char *savename
		,char *user,char *password)
{
	memset(opt,0x00,sizeof(FTP_OPT));
	sprintf(opt->local_file_path,"%s",localpath);
	sprintf(opt->user_key,"%s:%s",user,password);
	sprintf(opt->tmp_name,"%s.tmp",savename);
	sprintf(opt->save_name,"%s",savename);
	sprintf(opt->local_name,"%s",localfile);
	sprintf(opt->url,"ftp://%s:%s//%s/%s",ip,port,savepath,opt->tmp_name);
	return opt;
}

FTP_OPT *Get_download_ftp_opt(FTP_OPT *opt,char *ip,char *port
		,char *remotepath,char *remotefile,char *savepath,char *savename
		,char *user,char *password)
{
	sprintf(opt->url,"ftp://%s:%s//%s/%s",ip,port,remotepath,remotefile);
	sprintf(opt->local_file_path,"%s",savepath);
	sprintf(opt->user_key,"%s:%s",user,password);
	sprintf(opt->tmp_name,"%s.tmp",savename);
	sprintf(opt->save_name,"%s",savename);

	return opt;
}
