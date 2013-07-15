/**
 * cluries @ 2011
 * http://cdbit.com
 */ 


#ifndef __AMAZON_FILEMANAGER_H__
#define __AMAZON_FILEMANAGER_H__

#include "config.h"

#include "noncopyable.hpp"
#include "clloger.h"

namespace amazon
{

enum _FileManagerTargetMode
{
	kFileManagerTargetModeAppend  = 0,
	kFileManagerTargetModeInsert  = 1,
	kFileManagerTargetModeReplace = 2
};
typedef _FileManagerTargetMode FileManagerTargetMode;


class FileManager : public boost::noncopyable
{
public:
	FileManager(const char* source,const char* target,const char* inifile,FileManagerTargetMode mode) : 
			   m_source(source) , m_target(target) ,m_offset_file(inifile), m_target_mode(mode)
			   
	{
		init();
	}
	
	FileManager():m_source("source.csv"),m_target("target.csv"),m_offset_file("offset.ini"),m_target_mode(kFileManagerTargetModeAppend)
	{
		init();
	}

	~FileManager()
	{
		if (m_source_handler){
			fclose(m_source_handler);
		}

		if (m_target_handler) {
			fclose(m_target_handler);
		}
	}
public:
	inline AmazonAccount* fetch()
	{
		return &m_account;
	}
	bool next();
	void putTarget();	


	static size_t contents(const char* filename,char* dest,size_t maxsz);
private:
	void init();

	void readLineOffset()
	{
		m_line_offset = 0;
		FILE* offsetFileHandler = fopen(m_offset_file,"a+");
		fseek  (offsetFileHandler,0,SEEK_SET);
		fscanf (offsetFileHandler,"%d", &m_line_offset);
		fclose(offsetFileHandler);
	}

	void writeLineOffset()
	{
		FILE* offsetFileHandler = fopen(m_offset_file,"w+");
		fseek  (offsetFileHandler,0,SEEK_SET);
		fprintf(offsetFileHandler,"%d", m_line_offset);
		fclose(offsetFileHandler);
	}
private:
	const char* m_source;
	const char* m_target;
	const char* m_offset_file;
	
	FileManagerTargetMode m_target_mode  ;
	
	AmazonAccount 		  m_account	     ;
	char 				  m_account_fmt[AMAZON_ACCOUNT_FMT_SZ];
	int 				  m_line_offset  ;

	FILE*				  m_source_handler;
	FILE*				  m_target_handler;
};
		
}; //amazon 

#endif // end __AMAZON_FILEMANAGER_H__
