/**
 * cluries @ 2011
 * http://cdbit.com
 */ 
 
#ifndef __AMAZON_AMAZONIT_H__
#define __AMAZON_AMAZONIT_H__


#include "config.h"
#include "noncopyable.hpp"
#include <pcre.h>

#include <map>
#include <string>


namespace amazon
{

enum _AmazonitParams
{
	kPageBuffSize  = 1024 * 1024 * 4,
	kUserAgentSize = 1024,
	kFilePathSize  = 256,
	kPostFieldsSze = 1024*2,
};
typedef _AmazonitParams AmazonitParams;	

struct _PageContent
{
	size_t size;
	char   content[kPageBuffSize];
};
typedef _PageContent PageContent;

typedef std::map< std::string, std::string> requestParams;

class Amazonit : public boost::noncopyable
{
public:
	Amazonit()
	{
		init();
	}
public:
	int validate(AmazonAccount* account);
	int measurement();
public:
	int siginurl(char* url);
	int siginparams(const char* url,requestParams* params);
	int request(const char* url,requestParams* params = NULL);

	inline const PageContent& getPageContent()
	{
		return m_pageContent;
	}
private:
	void clearCookieJet();

private:
	static size_t writePageContent(void* contents,size_t size,size_t nmemb,void* userp);	
	static size_t pageHeaderWriteFunc(void *ptr, size_t size,size_t nmemb, void *stream);
	void init();

	void genRequestParamsString(requestParams* params);
	void genRequestCookieString();
	std::string urlEncode(const std::string& str);
	int  signinResult();
private:
	char 		m_userAgent[kUserAgentSize];
	char 		m_cookiejet[kFilePathSize];
	char 		m_cookiedir[kFilePathSize];
	char 		m_postFields[kPostFieldsSze];
	char		m_referer[kPostFieldsSze];
	char 		m_pageHeaaderResponse[kUserAgentSize];
	char 		m_cookiestring[kUserAgentSize];
	PageContent m_pageContent;
};

}; //amazon 

#endif // end __AMAZON_AMAZONIT_H__
