/**
 * cluries @ 2011
 * http://cdbit.com
 */ 


#ifndef __LIB_CLLOGER_H__
#define __LIB_CLLOGER_H__

#include "config.h"

namespace amazon
{

enum _ELogLevel {
    kLogInfo  = 0 ,
    kLogDebug = 1,
    kLogError = 2,
};
typedef _ELogLevel LogLevel;

enum _EInstanceAction {
    kActionGet      = 0,
    kActionDealloc  = 1
};
typedef _EInstanceAction InstanceAction;



class CLLoger {
public:
    ~CLLoger();
    
public:
    static CLLoger* instance(InstanceAction action = kActionGet);
    inline void logInfo(const char* file,const char* function,int line,const char* fmt,...) 
    {
        va_list args;
        va_start(args,fmt);
        this->log(kLogInfo, file, function, line, fmt,args);
        va_end(args);
    }
    
    inline void logDebug(const char* file,const char* function,int line,const char* fmt,...) 
    {
        va_list args;
        va_start(args,fmt);
        this->log(kLogDebug, file, function, line, fmt,args);
        va_end(args);
    }
    
    inline void logError(const char* file,const char* function,int line,const char* fmt,...) 
    {
        va_list args;
        va_start(args,fmt);
        this->log(kLogError, file, function, line, fmt ,args);
        va_end(args);
    }
    
    int discard(int beforeDays = 30) ;
private:
    void initLoger();
    void log(LogLevel level,const char* file,const char* function,int line,const char* fmt,va_list args) ;
    char* filename(char* dest,const char* file);
private:
    CLLoger();
    
    CLLoger( const CLLoger& );
    const CLLoger& operator=( const CLLoger& );
private:
    enum{
        kFilenameMax =  128                 ,
        kMessageMax  =  4096				,
		kLogDirBufSz =  512
    }                   m_bufssize          		;
    char                m_filename[kFilenameMax]    ;
    char                m_message[kMessageMax]      ;
    pthread_mutex_t     m_mutex             		;
    static const char*  m_fnameTemplate    			;
	char		    	m_log_dir[kLogDirBufSz]		;
};

}; // namespace amazon

#define LOG_E(fmt) amazon::CLLoger::instance()->logError(__FILE__, __FUNCTION__, __LINE__, (fmt));
#define LOG_I(fmt) amazon::CLLoger::instance()->logInfo(__FILE__, __FUNCTION__, __LINE__, (fmt));
#define LOG_D(fmt) amazon::CLLoger::instance()->logDebug(__FILE__, __FUNCTION__, __LINE__, (fmt));

#define LOG_INFO(fmt,...)  amazon::CLLoger::instance()->logInfo(__FILE__, __FUNCTION__, __LINE__, (fmt) , __VA_ARGS__);
#define LOG_DEBUG(fmt,...) amazon::CLLoger::instance()->logDebug(__FILE__, __FUNCTION__, __LINE__,(fmt) , __VA_ARGS__);
#define LOG_ERROR(fmt,...) amazon::CLLoger::instance()->logError(__FILE__, __FUNCTION__, __LINE__,(fmt) , __VA_ARGS__);


#endif /* defined(__LIB_CLLOGER_H__) */

