#ifndef THREAD_FACILITY_BASE_OCN_H
#define THREAD_FACILITY_BASE_OCN_H

namespace ocn
{
namespace base
{
namespace facility
{
class Thread
{
public:
    Thread();
    virtual ~Thread();

    virtual void run() = 0;

    void setRunning(bool running);

    bool running();

private:
    volatile bool m_running;
};

}
}
}

#endif // THREAD_FACILITY_BASE_OCN_H
