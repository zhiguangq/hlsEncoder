#ifndef EXCEPTION_FACILITY_BASE_OCN_H
#define EXCEPTION_FACILITY_BASE_OCN_H

#include <exception>
#include <boost/thread/mutex.hpp>

#if defined WIN32
#include <windows.h>
#include <dbghelp.h>
#elif defined __linux__
#include <execinfo.h>
#include <signal.h>
#elif defined __sun__
#include <ucontext.h>
#include <dlfcn.h>
#else
#error Platform required.
#endif

namespace ocn
{
namespace base
{
namespace facility
{

class Exception : public std::exception
{
public:
    Exception();
    Exception(const std::string &what);
    virtual ~Exception() throw();

    std::string getStackTrace(bool skipSelfStack = true);

    virtual const char *what() const throw();

protected:
    void traceStack(std::ostream &out);
    Exception &operator = (Exception const &);

#if defined WIN32
    void walkStack(HANDLE thread, CONTEXT &context);
    void showFrame(STACKFRAME64 &frame);

    HANDLE m_process;
#elif defined __sun__
    static int showFrame(uintptr_t pc, int signal, void *useragr);
#endif

    std::ostream *m_out;
    std::string m_stack;
    const std::string m_what;
    int m_depth;
    static int m_skip;
    static boost::mutex m_mutex;
private:
    std::string parse_symbol(const char* symbol);
};


#define EXCEPTION(xException, baseException) \
    class xException \
        : public baseException \
    { \
    public: \
        xException() \
            : baseException()\
        {++m_depth;} \
         \
        xException(const std::string &what) \
            : baseException(what) \
        {++m_depth;} \
    } /* 故意丢失分号，这样，用户必须添加分号，宏定义就和语句的格式一样了。 */

// 预定义异常类
EXCEPTION(OutOfBoundException, Exception);              // 数组越界，等……
EXCEPTION(ItemNotFoundException, Exception);            // 在vector，list，自定义容器中，没有找到该项目，等……
EXCEPTION(NotImplementedException, Exception);          // 功能还未实现。
EXCEPTION(NullPointerException, Exception);             // 解引用空指针。
EXCEPTION(DereferenceFailException, Exception);         // 解引用失败。
EXCEPTION(FileNotOpenException, Exception);             // 文件没有打开（未找到，没有权限……）。

}
}
}

#endif // EXCEPTION_FACILITY_BASE_OCN_H
