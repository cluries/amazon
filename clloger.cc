/**
 * cluries @ 2011
 * http://cdbit.com
 */ 

#include "clloger.h"

namespace amazon
{

const char* CLLoger::m_fnameTemplate = "yyyy_MM_dd.log";

CLLoger::CLLoger()
{
    pthread_mutex_init(&m_mutex, NULL);
    bzero(m_message, kMessageMax);
    initLoger();
}

CLLoger::~CLLoger()
{
    
}

void CLLoger::initLoger()
{
	bzero(m_log_dir,kLogDirBufSz);
	int rpret = prunpath(m_log_dir,kLogDirBufSz,"/logs");
	if(rpret < 1) {
		LOG_E("****Logger can not get runpath,init failed,logger can not wirte log file****\n");
		return;
	}
	
    if (access(m_log_dir, 0) == -1 ) {
        mkdir(m_log_dir, 0777);
    }
}


CLLoger* CLLoger::instance(InstanceAction action)
{
    static CLLoger* instance = NULL;
    if (action == kActionGet) {
        instance = instance == NULL ? new CLLoger() : instance;
    } else {
        if (NULL != instance) {
            delete instance ; instance = NULL;
        }
    }
    
    return instance;
}


void CLLoger::log(LogLevel level,const char* file,const char* function,int line,const char* fmt,va_list args) 
{
    if (pthread_mutex_lock(&m_mutex)) {
        perror("logger mutex lock faied");
        return;
    }
    bzero(m_message, kMessageMax);
    char filename[kFilenameMax];
    this->filename(filename, file);
    
    time_t current = time(0);
    tm* stm = localtime(&current );
    
    stm->tm_year += 1900;
    ++stm->tm_mon;
    sprintf(m_message ,"[%s] [%04d-%02d-%02d %02d:%02d:%02d] [%s %s %d] ",
            ((kLogDebug==level ? "DEBUG" : (kLogError == level ? "ERROR" : "INFO ")))
            , stm->tm_year,stm->tm_mon,stm->tm_mday,stm->tm_hour,stm->tm_min,stm->tm_sec,filename,function,line);
    int len = strlen(m_message);
    vsprintf(m_message+len,fmt,args);
    len     = strlen(m_message);
    *(m_message + len)      = '\n'  ;
    len += 1;
    sprintf(m_filename, "%s/%04d_%02d_%02d.log",m_log_dir,stm->tm_year,stm->tm_mon,stm->tm_mday);
    
    if (level == kLogError || level == kLogInfo) {
        FILE* fileHandler = fopen(m_filename, "a+");
        if (fileHandler != NULL) {
            fwrite(m_message, sizeof(char), len, fileHandler);
            fclose(fileHandler);
        }
    }
    printf("%s",m_message);
    
    
    pthread_mutex_unlock(&m_mutex);
}


int CLLoger::discard(int beforeDays) 
{
    if (beforeDays < 1) {
        return 0;
    }
    
    struct dirent* dptr = NULL;
    DIR*   dir = NULL;
    dir = opendir(m_log_dir);
    
    if (NULL == dir) {
        return -1;
    }
    
    int ret = 0;
    tm stm;
    time_t timestamp;
    char file[kFilenameMax];
    while ((dptr = readdir(dir)) != NULL) {
        
        if (!strncmp(".", dptr->d_name, 1) || !strncmp("..", dptr->d_name, 2)) {
            continue;
        }
        
        sscanf(dptr->d_name, "%04d_%02d_%02d.log",&stm.tm_year,&stm.tm_mon,&stm.tm_mday);
        stm.tm_year -= 1900;
        --stm.tm_mon;
        
        timestamp = mktime(&stm);
        
        if(time(0) - timestamp > beforeDays * 24 * 60 *60  ) {
            sprintf(file, "%s/%s",m_log_dir,dptr->d_name);
            this->logInfo(__FILE__, __FUNCTION__, __LINE__, "delete log file:%s",dptr->d_name);
            if(unlink(file)){
                this->logInfo(__FILE__, __FUNCTION__, __LINE__, "delete log file:%s failed",dptr->d_name);
            }
        }
        
        ++ret;
    }
    
    return ret;
}


char* CLLoger::filename(char* dest,const char *file)
{
    int len = strlen(file);
    int i = len;
    while (i > 0) {
        --i;
        if (*(file + i) == '/') {
            break;
        }
    }
    
    if (i == 0) {
        strncpy(dest, file, kFilenameMax);
    } else {
        memcpy(dest, file + i + 1, len - i);
    }
    
    return dest;
}


} //namespace amazon
