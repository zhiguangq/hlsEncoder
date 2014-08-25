#ifndef HASH_FACILITY_BASE_OCN_H
#define HASH_FACILITY_BASE_OCN_H

#include <string>
#include <iostream>  
#include <sstream>

namespace ocn
{
namespace base
{
namespace facility
{

class Hash
{
public:
    static unsigned int BKDRHash(char *str)
	{
		unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
		unsigned int hash = 0;

		while (*str)
		{
			hash = hash * seed + (*str++);
		}

		return (hash & 0x7FFFFFFF);
	}
	
    static unsigned int SDBMHash(char *str)
	{
		unsigned int hash = 0;

		while (*str)
		{
			// equivalent to: hash = 65599*hash + (*str++);
			hash = (*str++) + (hash << 6) + (hash << 16) - hash;
		}

		return (hash & 0x7FFFFFFF);
	}

};

}
}
}

#endif
