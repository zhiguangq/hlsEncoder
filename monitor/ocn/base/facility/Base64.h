#ifndef BASE64_FACILITY_BASE_OCN_H
#define BASE64_FACILITY_BASE_OCN_H

#include <string>
#include <iostream>  
#include <sstream>  

namespace ocn
{
namespace base
{
namespace facility
{

class Base64
{
public:
    static bool Base64Encode(const std::string& input, std::string* output);
    static bool Base64Decode(const std::string& input, std::string* output);

};

}
}
}

#endif
