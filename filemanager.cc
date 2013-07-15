/**
 * cluries @ 2011
 * http://cdbit.com
 */ 


#include "filemanager.h"

#include <iostream>

namespace amazon
{

void FileManager::init()
{
	bzero(&m_account, sizeof(AmazonAccount));
	bzero(m_account_fmt, AMAZON_ACCOUNT_FMT_SZ);

	readLineOffset();

	m_source_handler = fopen(m_source, "r");
	if (!m_source_handler) {
		LOG_ERROR("source file:%s not exist,program exit\n", m_source );
		exit(-1);	
	}

	//set source handler to xxx
	if (m_line_offset > 0) {
		int i = 0;
		while(i++ < m_line_offset && fgets(m_account_fmt, AMAZON_ACCOUNT_FMT_SZ, m_source_handler));
	}
	
	switch(m_target_mode)
	{
	case kFileManagerTargetModeAppend:
		m_target_handler = fopen(m_target, "a+");
		break;
	case kFileManagerTargetModeInsert:
		m_target_handler = fopen(m_target, "a+");
		break;		
	default:
		m_target_handler = fopen(m_target, "w");
		break;
	}

	if (!m_target_handler) {
		LOG_ERROR("target file:%s can not open or create,program exit\n", m_target );
		exit(-1);
	}
}

bool FileManager::next()
{ 
    if(fgets(m_account_fmt, AMAZON_ACCOUNT_FMT_SZ, m_source_handler)){
    	++m_line_offset;
    	writeLineOffset();
    	bzero(&m_account, sizeof(AmazonAccount));
    	char* p = strtok(m_account_fmt, ",");
    	strncpy(m_account.email, p, kAmazonAccountEmailSize);
    	p = strtok(NULL, ",");
    	strncpy(m_account.password, p, kAmazonAccountPasswordSize);
    	return true;
    }
    
	return false;
}

void FileManager::putTarget()
{
	size_t fmtsz = amazon_account_format(m_account_fmt, &m_account);
	fwrite(m_account_fmt, sizeof(char), fmtsz, m_target_handler);
}

size_t FileManager::contents(const char *filename, char* dest ,size_t maxsz)
{
	if (access(filename, F_OK) == -1) {
		LOG_ERROR("file:%s is not exist\n", filename);
		return 0;
	}

	FILE* file = fopen(filename, "r");
	if (!file){
		LOG_ERROR("open file:%s faield\n", filename);
	}

	fseek(file, 0, SEEK_SET);
	size_t readed = fread(dest, sizeof(char), maxsz, file);
	fclose(file);

	return readed;
}

};
