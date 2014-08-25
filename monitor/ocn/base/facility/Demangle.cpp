#include <typeinfo>
#include <string>
#include <ocn/base/facility/Demangle.h>

#ifdef _MSC_VER

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include <windows.h>
#include <Dbghelp.h>
#pragma comment(lib, "DbgHelp")

#elif defined __GNUC__
#include <execinfo.h>
#include <cxxabi.h>
#include <stdlib.h>
#endif

namespace ocn
{
namespace base
{
namespace facility
{

std::string Demangle::demangle(const std::string& name)
{
	return impl_demangle(name);
}
template<typename T>
std::string Demangle::demangle()
{
	return impl_demangle(typeid(T).name());
}

std::string Demangle::impl_demangle(const std::string& name)
{
#ifdef _MSC_VER
    char buffer[1024];
    DWORD length = UnDecorateSymbolName(name.c_str(), buffer, sizeof(buffer), 0);
    if (length > 0)
        return std::string(buffer, length);
    else
        return name;
#elif defined __GNUC__
	size_t funcnamesize = 1;
	char* funcname = static_cast<char*>(malloc(funcnamesize));
	int status;
	char* ret = abi::__cxa_demangle(name.c_str(), funcname, &funcnamesize, &status);
    if (0 == status)
    {
    	//demangle success
    	std::string result(ret);
    	free(ret);
    	return result;
    }
    else
    {
    	//demangle fail
    	free(funcname);
    	return name;
    }
#endif
}

}
}
}
