#include <ocn/base/facility/ErrorMsg.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

#if defined WIN32
#include <windows.h>
#elif defined __linux__ || defined __sun__
#include <errno.h>
#else
#error Platform required.
#endif

using namespace boost;

namespace ocn
{
namespace base
{
namespace facility
{

std::string getErrorMsg()
{
#if defined WIN32
    std::string msg;
    CHAR *msgBuf;
    DWORD error = GetLastError();
    if (!FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    error,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR)&msgBuf,
                    0,
                    NULL))
    {
        return msg;
    }

    msg = msgBuf;

    std::string::size_type pos = msg.find(("\n"));
    if (pos != std::string::npos)
    {
        msg = msg.substr(0, pos);
    }

    LocalFree((HLOCAL)msgBuf);

    return trim_copy(lexical_cast <std::string> (error) + (": ") + msg);
#elif defined __linux__ || defined __sun__
    return trim_copy(lexical_cast<std::string> (errno) + (": ")+ (strerror(errno)));
#else
#error Platform required.
#endif
}

}
}
}
