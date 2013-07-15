/**
 * cluries @ 2011
 * http://cdbit.com
 */ 


#ifndef __AMAZON_CONFIG_H__
#define __AMAZON_CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <libproc.h>

#include <string>

#if defined(__APPLE__) || defined(__MACOSX__)
#include <mach-o/dyld.h>
#endif

#include "clloger.h"


namespace amazon
{


enum _AmazonAccountFieldSize{
	kAmazonAccountEmailSize 	= 128,
	kAmazonAccountPasswordSize 	= 64
};
typedef _AmazonAccountFieldSize AmazonAccountFieldSize;

struct _AmazonAccount
{
	char  email[kAmazonAccountEmailSize]		;
	char  password[kAmazonAccountPasswordSize]	;
};
typedef _AmazonAccount AmazonAccount;

#define AMAZON_ACCOUNT_FMT_SZ (sizeof(AmazonAccount) + 1)


inline size_t amazon_account_format(char* dest,AmazonAccount* aa)
{
	bzero(dest, AMAZON_ACCOUNT_FMT_SZ);
	sprintf(dest, "%s,%s",aa->email,aa->password);
	return strlen(dest);
}

inline char* trim(char* source)
{
	return source;
}

inline int prunpath(char* dest,size_t destsz,const char* append)
{
	bzero(dest, destsz);
#if defined(__linux__) || defined(__linux)
	int sz = readlink( "/proc/self/exe", dest, destsz);
	if( sz == -1){
		printf("Error when get runpath because:%d - %s",errno,strerror(errno));
		return sz;
	}
#elif defined(__FreeBSD__)
	//do freebsd
#elif defined(__APPLE__) || defined(__MACOSX__)
	uint32_t destusz = static_cast <uint32_t> (destsz);
	uint32_t sz = _NSGetExecutablePath(dest,&destusz);
	if(sz) {
		printf("buffer to smal,need size %u",sz);
		return -1;
	}else{
		sz = strlen(dest);
		while(--sz > 0 ) {
			if(*(dest+sz) == '/') {
				*(dest+sz) = '\0';
				if( sz >= 1 && *(dest + sz - 1) == '.') {
					*(dest + sz - 1) = '\0';
				}
				break;
			}
		}
	}
#elif defined(__sun) || defined(sun)
	
#endif
	

	if(append != NULL) {
		size_t apdsz = strlen(append);
		if( destsz >= sz + apdsz) {
			strcat(dest,append);
			return sz + apdsz;
		}
	}
	

	return sz ;
}

static inline std::string &ltrim(std::string &s) { 
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace)))); 
        return s; 
} 

// trim from end 
static inline std::string &rtrim(std::string &s) { 
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end()); 
        return s; 
} 

static inline std::string &trim(std::string &s) { 
        return ltrim(rtrim(s)); 
} 
 	
}; // amazon

#endif
