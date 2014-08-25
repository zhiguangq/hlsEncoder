#include "Exception.h"
#include <ocn/base/facility/Demangle.h>
#include <sstream>
#include <cassert>

using namespace std;
using namespace boost;

namespace ocn
{
namespace base
{
namespace facility
{

boost::mutex Exception::m_mutex; // 此语句会造成一个内存泄露的警告，可忽略。

#if defined WIN32
int Exception::m_skip = 3;
#elif defined __linux__
#if __NO_INLINE__
int Exception::m_skip = 2;
#else
int Exception::m_skip = 1;
#endif
#elif defined __sun__
int Exception::m_skip = 1;
#else
#error Platform required.
#endif

Exception::Exception() :
    m_depth(0)
{
    ostringstream oss;
    traceStack(oss);
    m_stack = oss.str();
}

Exception::Exception(const std::string &what) :
    m_what(what), m_depth(0)
{
    ostringstream oss;
    traceStack(oss);
    m_stack = oss.str();
}

Exception::~Exception() throw()
{
}

#if defined WIN32

void Exception::traceStack(ostream &out)
{
    mutex::scoped_lock lk(m_mutex);

    m_process = GetCurrentProcess();
    m_out = &out;
    HANDLE thread = GetCurrentThread();
    CONTEXT context;
    ZeroMemory(&context, sizeof(context));
    context.ContextFlags = CONTEXT_FULL;
    GetThreadContext(thread, &context);
    walkStack(thread, context);
    CloseHandle(thread);
    CloseHandle(m_process);
}

void Exception::walkStack(HANDLE thread, CONTEXT &context)
{
    SymInitialize(m_process, 0, true);

    STACKFRAME64 stackFrame;
    ZeroMemory(&stackFrame, sizeof(stackFrame));
    stackFrame.AddrPC.Offset = context.Eip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Ebp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;

    while(StackWalk64(
                    IMAGE_FILE_MACHINE_I386,
                    m_process,
                    thread,
                    &stackFrame,
                    &context,
                    0,
                    SymFunctionTableAccess64,
                    SymGetModuleBase64,
                    NULL))
    {
        showFrame(stackFrame);
        *m_out << endl;
    }

    SymCleanup(m_process);
}

void Exception::showFrame(STACKFRAME64 &frame)
{
    const int maxNameLen = 512;
    SYMBOL_INFO *symbol = static_cast <SYMBOL_INFO *> (malloc(sizeof(SYMBOL_INFO) + maxNameLen));
    ZeroMemory(symbol, sizeof(SYMBOL_INFO) + maxNameLen);
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = maxNameLen;

    IMAGEHLP_LINE64 line;
    ZeroMemory(&line, sizeof(line));
    line.SizeOfStruct = sizeof(line);

    if (frame.AddrPC.Offset == 0)
    {
        *m_out << "Error: EIP=0\n";
        free(symbol);
        return;
    }

    *m_out << "\tat ";

    DWORD64 offset;
    if (SymFromAddr(m_process, frame.AddrPC.Offset, &offset, symbol))
        *m_out << symbol->Name;
    else
        *m_out << "(no symbol name)";

    DWORD displacement;
    if (SymGetLineFromAddr64(m_process, frame.AddrPC.Offset, &displacement, &line))
        *m_out << "(" << line.FileName << ":" << line.LineNumber << ")";
    else
        *m_out << "(no file name)";

    free(symbol);
}

#elif defined __linux__

void Exception::traceStack(ostream &out)
{
    mutex::scoped_lock lk(m_mutex);

    void *array[25];
    int nSize = backtrace(array, 25);
    char **symbols = backtrace_symbols(array, nSize);

    for (int i = 0; i < nSize; i++)
    {
        out << "\tat " << parse_symbol(symbols[i]) << endl;
    }

    free(symbols);
}

#elif defined __sun__

void Exception::traceStack(ostream &out)
{
    mutex::scoped_lock lk(m_mutex);

    m_out = &out;
    ucontext_t ucp;
    int n = getcontext(&ucp);
    if (n == 0)
    {
        walkcontext(&ucp, showFrame, this);
    }
}

int Exception::showFrame(uintptr_t pc, int signal, void *useragr)
{
    Dl_info di;
    dladdr((void *)pc, &di);
    Exception *e = static_cast <Exception *> (useragr);
    *e->m_out << "\tat " << di.dli_fname << "(" << di.dli_sname << ") [0x" << hex <<
        static_cast <char *> (di.dli_saddr) - static_cast <char *> (di.dli_fbase) << "]" << endl;
    return 0;
}

#else
#error Platform required.
#endif

std::string Exception::parse_symbol(const char* symbol)
{
	std::string tmp = symbol;
	std::string::size_type begin_pos = tmp.find_first_of('(');
	std::string::size_type end_pos = std::string::npos;
    if (begin_pos != std::string::npos)
    {
    	end_pos = tmp.find_first_of(('+'), begin_pos);
    }
    else
    {
    	// 对于其他情况，对整个输入进行处理
    	return Demangle::demangle(symbol);
    }
	std::string ret(tmp.c_str(), begin_pos + 1);
	if (end_pos != std::string::npos)
	{
		ret.append(Demangle::demangle(tmp.substr(begin_pos + 1, end_pos - begin_pos - 1)));
		ret.append(tmp.substr(end_pos));
	}
	else
	{
		ret.append(Demangle::demangle(tmp.substr(begin_pos + 1)));
	}

	return ret;
}
std::string Exception::getStackTrace(bool skipSelfStack)
{
    std::string exceptionName = Demangle::demangle(typeid(*this).name());

    if (skipSelfStack)
    {
        if (m_stack.empty())
        {
            return exceptionName + std::string((": ") + m_what);
        }
        else
        {
            int count = 0;
            string::size_type offset = 0;
#if defined __sun__ && !__NO_INLINE__
            m_depth = 0;
#endif
            while (count < m_depth + m_skip)                // 跳过多余的函数调用信息。
            {
                ++count;
                offset = m_stack.find_first_of(("\n"), offset);
                if (offset == string::npos)
                {
                    break;
                }
                ++offset;
            }
            return exceptionName + std::string((": ")) + (m_what)+ ("\n")+ m_stack.substr(offset);
        }
    }
    else
    {
        if (m_stack.empty())
        {
            return exceptionName + std::string((": ")) + (m_what);
        }
        else
        {
            return exceptionName + std::string((": ")) + (m_what)+ ("\n")+ m_stack;
        }
    }
}

const char *Exception::what() const throw()
{
    return m_what.c_str();
}

}
}
}
