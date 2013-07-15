/**
 * cluries @ 2011
 * http://cdbit.com
 */ 

#ifndef __AMAZON_PROPERTY_H__
#define __AMAZON_PROPERTY_H__

#include "noncopyable.hpp"
#include "config.h"

#include <map>

typedef char* property_key	;
typedef char* property_val 	;
typedef std::map<property_key,property_val> properties;

namespace amazon
{

class Property : public boost::noncopyable
{
public:
	Property(){}
	~Property();
public:
	inline size_t size()
	{
		return m_properties.size();
	}
	
	bool has(const property_key key) const;
	bool set(const property_key key,const property_val val);
	bool add(const property_key key,const property_val val);
	const property_val get(const property_key key,const property_key defaults);
	size_t del(const property_key key);
private:
	properties m_properties; 
};
		
}; //amazon

#endif
