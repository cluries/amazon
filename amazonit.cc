/**
 * cluries @ 2011
 * http://cdbit.com
 */ 
 
#include "amazonit.h"

#include <curl/curl.h>
#include "filemanager.h"


namespace amazon
{	

static requestParams 	g_cookies;

void Amazonit::init()
{
	bzero(m_userAgent, kUserAgentSize);
	bzero(m_cookiejet, kFilePathSize) ;
	bzero(m_cookiedir, kFilePathSize) ;
	bzero(m_postFields, kPostFieldsSze);
	bzero(m_referer,kPostFieldsSze);
	bzero(m_cookiestring, kUserAgentSize);

	char runpath[kFilePathSize];bzero(runpath, kFilePathSize);

	if(prunpath(runpath, kFilePathSize, NULL)<1){
		LOG_E("can not get run path");
		return ;	
	}

	sprintf(m_cookiedir, "%s/runtime",runpath);
	
	if(access(m_cookiedir,F_OK)==-1) {
		mkdir(m_cookiedir,0777);
	}
	
	sprintf(m_cookiejet, "%s/runtime/cookie_%ld",runpath,time(0));
	strcat(runpath, "/config/UserAgent");
	FileManager::contents(runpath, m_userAgent, kUserAgentSize);
}	

int Amazonit::measurement()
{
	request("http://www.weibo.com/");
	printf("%s\n",m_pageContent.content );
	return 0;
}

int Amazonit::validate(AmazonAccount* account)
{
	std::string user(account->email);
	user = trim(user);

	std::string pwd(account->password);
	pwd = trim(pwd);
	 
	LOG_INFO("validate |%s %s|", user.c_str() , pwd.c_str());
	clearCookieJet();
	
	char url[1024];bzero(url, 1024);
	if(siginurl(url)!=0){
		LOG_E("Can not get login url");
		return -1;
	}

	if(request(url) != CURLE_OK){
		LOG_ERROR("can not open login url:%s",url);
		return -2;
	}	

	requestParams params;
	params.insert(requestParams::value_type(std::string("email"),user));
	params.insert(requestParams::value_type(std::string("password"),pwd));
	siginparams(url, &params);

	const char* dosigninUrl = "https://www.amazon.com/ap/signin";
	if (request(dosigninUrl,&params) != CURLE_OK) {
		LOG_E("when do login some error has occ..");
		return -3;
	}

	return signinResult();
}


int Amazonit::signinResult()
{
	/*
	char outfile[32];sprintf(outfile, "runtime/outfile_%ld.html",time(0));
	FILE* f = fopen(outfile, "w+");
	if (f)
	{
		fwrite(m_pageContent.content, sizeof(char), m_pageContent.size, f);
		fclose(f);
	}
	*/

	if (m_pageContent.size < 1024*5) {
		return 0;
	}


	int pcreErrOffset = -1;
	const char* pcreErrMessage = NULL;
	const char* pattern = "sign out";	
	pcre* regex = pcre_compile(pattern, PCRE_CASELESS, &pcreErrMessage,	&pcreErrOffset, NULL);	
	if (!regex) {
		LOG_ERROR("PCRE compile failed at offset %d : %s", pcreErrOffset,pcreErrMessage);
		return -1;
	}

	int ovector[40];
	return pcre_exec(regex, NULL, m_pageContent.content, m_pageContent.size, 0, 0, ovector, 40);
}

size_t Amazonit::writePageContent(void* contents,size_t size,size_t nmemb,void* userp)
{
	size_t rsize = size * nmemb;
	PageContent* pagecontent = (PageContent*)userp;
	memcpy(pagecontent->content+pagecontent->size,contents,rsize);
	pagecontent->size += rsize;
	
	return rsize;
}

size_t Amazonit::pageHeaderWriteFunc(void *ptr, size_t size,size_t nmemb, void *stream)
{
	char* strings = (char*)ptr;

	int pcreErrOffset = -1;
	const char* pcreErrMessage = NULL;
	const char* pattern = "Set-cookie\\s*:\\s*(.*?)=(.*?);";	
	pcre* regex = pcre_compile(pattern, PCRE_CASELESS, &pcreErrMessage,	&pcreErrOffset, NULL);	
	if (!regex) {
		LOG_ERROR("PCRE compile failed at offset %d : %s", pcreErrOffset,pcreErrMessage);
		return -1;
	}

	int ovector[10];
	if (pcre_exec(regex, NULL, strings, size * nmemb, 0, 0, ovector, 10) > 0) {
		char tmp[512];bzero(tmp, 512);
		memcpy(tmp, strings + ovector[1*2], ovector[1*2+1] - ovector[1*2]);
		std::string key(tmp);

		bzero(tmp, 512);
		memcpy(tmp, strings + ovector[2*2], ovector[2*2+1] - ovector[2*2]);
		std::string val(tmp);
		
		g_cookies[key] = val;
	}

	return size * nmemb;
}

int Amazonit::siginparams(const char* url,requestParams* params)
{
	int pcreErrOffset = -1;
	const char* pcreErrMessage = NULL;
	const char* pattern = "<\\s*input\\s+type\\s*=\\s*\"hidden\"\\s+name\\s*=\\s*\"(.*?)\"\\s+value\\s*=\\s*\"(.*?)\".*?\\/>";	
	pcre* regex = pcre_compile(pattern, PCRE_CASELESS, &pcreErrMessage,	&pcreErrOffset, NULL);	
	if (!regex) {
		LOG_ERROR("PCRE compile failed at offset %d : %s", pcreErrOffset,pcreErrMessage);
		return -1;
	}

	int ovector[10];
	int rs;
	int startexec = 0;
	char tmp[128];
	int glen = 0; 
	while((rs = pcre_exec(regex, NULL, m_pageContent.content, m_pageContent.size, startexec, 0, ovector, 10)) > 0) {
		glen = ovector[1*2 + 1] - ovector[1 * 2];
		bzero(tmp,128);
		memcpy(tmp,m_pageContent.content+ovector[1 * 2], glen>128 ? 128 : glen);
		std::string key(tmp);
		
		bzero(tmp,128);
		glen = ovector[2*2 + 1] - ovector[2 * 2];
		memcpy(tmp,m_pageContent.content+ovector[2 * 2], glen>128 ? 128 : glen);
		std::string val(tmp);
		params->insert(requestParams::value_type(key,val));	
    	startexec = ovector[1];
	}
	pcre_free(regex);  

	params->insert(requestParams::value_type(std::string("create"),std::string("0")));
	params->insert(requestParams::value_type(std::string("x"),std::string("52")));
	params->insert(requestParams::value_type(std::string("y"),std::string("4")));

	return 0;
}

int Amazonit::siginurl(char* url)
{
	if (request("http://www.amazon.com/") != CURLE_OK) {
		return -1;
	}

	int pcreErrOffset = -1;
	const char* pcreErrMessage = NULL;
	const char* pattern = "href=\"(https://www.amazon.com/ap/signin\?.*?)\"";	
	pcre* regex = pcre_compile(pattern, PCRE_CASELESS, &pcreErrMessage,	&pcreErrOffset, NULL);
	if (!regex) {
		LOG_ERROR("PCRE compile failed at offset %d : %s", pcreErrOffset,pcreErrMessage);
		return -1;
	}

	int ovector[10];
	int rs = pcre_exec(regex, NULL, m_pageContent.content, m_pageContent.size, 0, 0, ovector, 10);
	if (!rs || rs < 1) {	
		LOG_E("not found matched url");
		return -2;
	}
	pcre_free(regex);  
	memcpy(url, m_pageContent.content + ovector[1*2], ovector[1*2 + 1] -ovector[1*2]);

	return 0;
}

void Amazonit::genRequestParamsString(requestParams* params)
{
	bzero(m_postFields, kPostFieldsSze);

	if (params->empty()) {
		return;
	}

	requestParams::iterator iter = params->begin();
	size_t sz = 0;
	while(iter != params->end() ) {
		std::string key = iter->first;
		std::string val = iter->second;

		key = urlEncode(key);
		val = urlEncode(val);

		if (sz + key.size() + val.size() + 2 > kPostFieldsSze) {
			LOG_E("Postfields buff too small");
			break;
		}

		strcat(m_postFields, key.c_str());
		strcat(m_postFields, "=");
		strcat(m_postFields, val.c_str());
		strcat(m_postFields, "&");
		++iter;

		sz += key.size() + val.size() + 2 ;
	}
}

int  Amazonit::request(const char* url,requestParams* params)
{	
	bzero(&m_pageContent, sizeof(PageContent));
	CURL* curl;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	
	if(!curl) {
		return -1;
	}
	
	curl_easy_setopt(curl,CURLOPT_URL,url);
	curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1);
	curl_easy_setopt(curl,CURLOPT_USERAGENT,  m_userAgent);
	curl_easy_setopt(curl,CURLOPT_COOKIEFILE, m_cookiejet);
	curl_easy_setopt(curl,CURLOPT_COOKIEJAR , m_cookiejet);
	curl_easy_setopt(curl,CURLOPT_COOKIESESSION,0);
	//curl_easy_setopt(curl,CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,writePageContent);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,(void*)&m_pageContent);
	curl_easy_setopt(curl,CURLOPT_REFERER,m_referer);
	curl_easy_setopt(curl,CURLOPT_HEADERDATA, m_pageHeaaderResponse);
	curl_easy_setopt(curl,CURLOPT_HEADERFUNCTION,pageHeaderWriteFunc);

	if (params != NULL) {
		genRequestParamsString(params);
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS,m_postFields);
	}

	if (g_cookies.size() > 0) {
		genRequestCookieString();
		curl_easy_setopt(curl,CURLOPT_COOKIE, m_cookiestring);
	}
	
	CURLcode res = curl_easy_perform(curl);

	strncpy(m_referer,url,kPostFieldsSze);
	
	if(res != CURLE_OK) {
		printf("failed:%s",curl_easy_strerror(res));
		return -1;
	}
	curl_global_cleanup();
	
	return res;
}


void Amazonit::clearCookieJet()
{
	g_cookies.clear();

	struct dirent* dptr = NULL;
    DIR*   dir = NULL;
    dir = opendir(m_cookiedir);
    
    if (NULL == dir) {
        return ;
    }
   
    time_t timestamp;
    char file[kFilePathSize];
    while ((dptr = readdir(dir)) != NULL) {
        
        if (!strncmp(".", dptr->d_name, 1) || !strncmp("..", dptr->d_name, 2)) {
            continue;
        }
        
        sscanf(dptr->d_name, "cookie_%ld",&timestamp);
      
        
        if(time(0) - timestamp > 10  ) {
            sprintf(file, "%s/%s",m_cookiedir,dptr->d_name);
       
            if(unlink(file)){
         		LOG_ERROR("can not unlink file:%s", file);  		
            }
        }   
    }
    

    FILE* f = fopen(m_cookiejet, "w+");
    if (f) {
    	fclose(f);
    }
}

void Amazonit::genRequestCookieString()
{
	bzero(m_cookiestring, kUserAgentSize);
	if (g_cookies.size()<1) {
		return;
	}

	requestParams::iterator iter= g_cookies.begin();
	while(iter != g_cookies.end()) {
		strcat(m_cookiestring, iter->first.c_str());
		strcat(m_cookiestring, "=");
		strcat(m_cookiestring, iter->second.c_str());
		strcat(m_cookiestring, " ; ");
		++iter;
	}
}

std::string Amazonit::urlEncode(const std::string& str)
{
	std::string src = str;
	char hex[] = "0123456789ABCDEF";
	std::string dst;

	for (size_t i = 0; i < src.size(); ++i)
	{
		unsigned char cc = src[i];
		if (isascii(cc))
		{
			if (cc == ' ')
			{
				dst += "%20";
			}
			else
				dst += cc;
		}
		else
		{
			unsigned char c = static_cast<unsigned char>(src[i]);
			dst += '%';
			dst += hex[c / 16];
			dst += hex[c % 16];
		}
	}

	return dst;
}

	
};
