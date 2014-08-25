#include "Thread.h"

namespace ocn
{
namespace base
{
namespace facility
{

Thread::Thread() : m_running(false)
{
}

Thread::~Thread()
{
}

void Thread::setRunning(bool running)
{
    m_running = running;
}

bool Thread::running()
{
    return m_running;
}

}
}
}
