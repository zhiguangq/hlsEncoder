#ifndef CLOCK_FACILITY_BASE_OCN_H
#define CLOCK_FACILITY_BASE_OCN_H

#include <boost/thread/mutex.hpp>
//#include <com/alu/base/BaseHeader.h>
#include <ocn/base/facility/Exception.h>
#include <ocn/base/facility/Singleton.h>
//#include <com/alu/base/facility/GlobalMutex.h>
//#include <com/alu/base/facility/Thread.h>
//#include <com/alu/base/debug/rtslog.h>
//#include <com/alu/base/facility/SingletonLongevity.h> //unify lifetime

namespace ocn
{
namespace base
{
namespace facility
{

EXCEPTION(GetSystemClockErrorException, ocn::base::facility::Exception);

class Clock : public ocn::base::facility::Singleton<Clock>
{
    friend class ocn::base::facility::Singleton<Clock>;
    /*
    class Flywheel : public Thread, private boost::noncopyable
    {
    public:
        virtual void run();

        long long m_now;
    };

    typedef ThreadHolder <Flywheel> FlywheelThread;
    typedef Loki::SingletonHolder <FlywheelThread, Loki::CreateUsingNew, com::alu::base::facility::Die2nd,
        Loki::ClassLevelLockable, com::alu::base::facility::GlobalMutex> SFW;
        */

public:
    class EventHandler
    {
    protected:
        virtual ~EventHandler() {}

    public:
        virtual void onTimerChange(long long shift) = 0;
    };

    void setEventHandler(EventHandler *eh);

    void sleepSecond(long long second, bool UTC = false);
    void sleepMillisecond(long long millisecond, bool UTC = false);
    void sleepMicrosecond(long long microsecond, bool UTC = false);
    void sleepNanosecond(long long nanosecond, bool UTC = false);

    long long getSecond();
    long long getMillisecond();
    long long getMicrosecond();
    long long getNanosecond();

    /*
    long long getFastSecond();
    long long getFastMillisecond();
    long long getFastMicrosecond();
    long long getFastNanosecond();
    */

    static time_t timegm(tm *ptm);
    static int gmtime(const time_t *ptime, tm *presult);

    // 描述：把数值时间转换为UTC字符串表示的时间。线程安全。
    // 参数：[nanosecond]，数值时间，单位为纳秒。
    // 返回：UTC字符串时间，形如[20090729T100000Z]；出错返回空字符串。
    static std::string getUTCString(const long long &nanosecond);

    // 描述：把UTC字符串表示的时间转换为数值时间，线程安全。
    // 参数：[utcstr]，UTC字符串时间，形如[20090729T100000Z]。
    // 返回：数值时间，单位为纳秒；出错返回-1。
    static long long getUTCTime(const std::string &utcstr);

    long long getMonoTick();                    // 获得单调增长计数值。

    ~Clock();
private:
    Clock(long long sensitivty = 1000*1000*1000*10);
    
    long long getTime();                        // 获得系统的绝对时间。

    boost::mutex m_mutex;
    long long m_sensitivty;                     // 检测的时钟修改的检测灵敏程度。
    long long m_lastTick;                       // 上一个计数点。
    long long m_baseTick;                       // 计数基点。
    long long m_baseTime;                       // 时间基点。
    long long m_snapTime;                       // 时钟速算点。
    long long m_lastClockTime;                  // 上一个时钟值。
    bool m_flywheelStart;
    long long m_nowSecond;                      // 当前时间秒。
    long long m_nowMillisecond;                 // 当前时间毫秒。
    long long m_nowMicrosecond;                 // 当前时间微秒。
    long long m_nowNanosecond;                  // 当前时间纳秒。

    long long m_resumePoint;

    EventHandler *m_eh;
};

}
}
}

#endif
