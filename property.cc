/**
 * cluries @ 2011
 * http://cdbit.com
 */ 


#include "property.h"


namespace amazon
{

property_val property_keyorval_alloc(size_t size)
{
	if(size < 1){
		return NULL;
	}
	
	return new char[size + 1];
}


bool Property::has(const property_key key) const
{
	if(m_properties.empty()){
		return false;
	}
	
	properties::const_iterator iter = m_properties.begin();
	while(iter != m_properties.end()) {
		if(strcmp(key,iter->first) == 0) {
			return true;
		}
		
		++iter;
	}
	
	return false;
}

bool Property::set(const property_key key,const property_val val)
{
	size_t keysize = strlen(key) ;
	size_t valsize = strlen(val) ;
	
	property_key mkey = property_keyorval_alloc(keysize);
	property_val mval = property_keyorval_alloc(valsize);
	
	m_properties[mkey] = mval;
	
	return true;
}

bool Property::add(const property_key key,const property_val val)
{
	if(has(key)){
		printf("key %s is aready exists\n",key);
		return false;
	}
	size_t keysize = strlen(key) ;
	size_t valsize = strlen(val) ;
	
	property_key mkey = property_keyorval_alloc(keysize);
	property_val mval = property_keyorval_alloc(valsize);

	m_properties.insert(properties::value_type(mkey,mval));
	
	return true;
}

const property_val Property::get(const property_key key,const property_key defaults)
{
	if(m_properties.empty()){
		return defaults;
	}
	
	properties::iterator iter = m_properties.begin();
	while(iter != m_properties.end()) {
		if(strcmp(key,iter->first) == 0) {
			return iter->second;
		}
		++iter;
	}
	
	return defaults;
}

size_t Property::del(const property_key key)
{
	if(!has(key)) {
		printf("key:%s is not found ins properties",key);
		return -1;
	}
	
	properties::iterator iter = m_properties.begin();
	property_key mkey;
	property_val mval;
	while(iter != m_properties.end()) {
		if(strcmp(key,iter->first) == 0) {
			mkey = iter->first	;
			mval = iter->second	;
			delete [] mkey ;mkey  = NULL;
			delete [] mval ;mval  = NULL;
			m_properties.erase(iter);
			break; 
		}
		++iter;
	}
	
	return m_properties.size();
}

Property::~Property()
{
	if(m_properties.empty()){
		return;
	}
	
	properties::iterator iter = m_properties.begin();
	property_key key;
	property_val val;
	while(iter != m_properties.end()) {
		key = iter->first;
		val = iter->second;
		delete []key; key = NULL;
		delete []val; val = NULL;
		++iter;
	}
	
	m_properties.clear();
}

}; //amazon

