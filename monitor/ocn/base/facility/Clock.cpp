#include <stdio.h>
#include <cassert>
#include <time.h>
#include <boost/lexical_cast.hpp>
#include <ocn/base/facility/ErrorMsg.h>
#include <ocn/base/facility/Clock.h>
#include <ocn/base/facility/log.h>

#if defined __linux__ || defined __sun__
#include <errno.h>
#include <sys/time.h>
#endif

using namespace boost;
using namespace ocn::base::facility;

namespace ocn
{
namespace base
{
namespace facility
{

const long long US  = 1000LL;                       // 微秒
const long long MS  = 1000 * US;                    // 毫秒
const long long S   = 1000 * MS;                    // 秒
const long long MIN = 60 * S;                       // 分钟
const long long HR  = 60 * MIN;                     // 小时
const long long DAY = 24 * HR;                      // 天
const long long WEEK = 7 * DAY;                     // 周
/*
void Clock::Flywheel::run()
{
    while (running())
    {
        SC::Instance().m_nowNanosecond = SC::Instance().getNanosecond();
        SC::Instance().m_nowMicrosecond = SC::Instance().m_nowNanosecond / 1000;
        SC::Instance().m_nowMillisecond = SC::Instance().m_nowMicrosecond / 1000;
        SC::Instance().m_nowSecond = SC::Instance().m_nowMillisecond / 1000;
        SC::Instance().sleepMillisecond(1);
    }
}
*/

Clock::Clock(long long sensitivty)
    : m_sensitivty(sensitivty)
    , m_lastTick(0)
    , m_baseTick(getMonoTick())
    , m_baseTime(getTime())
    , m_snapTime(m_baseTime - m_baseTick)
    , m_lastClockTime(0)
    , m_flywheelStart(false)
    , m_nowSecond(0)
    , m_nowMillisecond(0)
    , m_nowMicrosecond(0)
    , m_nowNanosecond(0)
    , m_resumePoint(0)
    , m_eh(NULL)
{
}

Clock::~Clock()
{
}

void Clock::setEventHandler(EventHandler *eh)
{
    m_eh = eh;
}

void Clock::sleepSecond(long long second, bool UTC)
{
    sleepNanosecond(second * S, UTC);
}

void Clock::sleepMillisecond(long long millisecond, bool UTC)
{
    sleepNanosecond(millisecond * MS, UTC);
}

void Clock::sleepMicrosecond(long long microsecond, bool UTC)
{
    sleepNanosecond(microsecond * US, UTC);
}

void Clock::sleepNanosecond(long long nanosecond, bool UTC)
{
    if (UTC)
    {
        long long now = getNanosecond();
        long long duration = nanosecond - now;
        if (duration <= 0)
            return;
        nanosecond = duration;
    }

#if defined WIN32
    Sleep(static_cast <DWORD> (nanosecond / 1000000));
#elif defined __linux__ || defined __sun__
    usleep(static_cast <int> (nanosecond / 1000));
#else
#error Platform required.
#endif
}

long long Clock::getSecond()
{
    return getNanosecond() / S;
}

long long Clock::getMillisecond()
{
    return getNanosecond() / MS;
}

long long Clock::getMicrosecond()
{
    return getNanosecond() / US;
}

long long Clock::getNanosecond()
{
    mutex::scoped_lock lk(m_mutex);
    long long tick = getMonoTick();                                     // 获得计数量
    if (m_sensitivty < tick - m_lastTick)                           // 距上次更新时间超过
    {
        m_lastTick = tick;
        long long timeDelta = getTime() - m_baseTime;                   // 时间增量
        long long tickDelta = tick - m_baseTick;                        // 计数增量
        long long shift = timeDelta - tickDelta;                        // 计算时钟修改量
        if (shift < S * -2 || S * 2 < shift)                        // 超过修改范围2s
        {
            if (m_eh)
                m_eh->onTimerChange(shift);                         // 通告事件
            m_snapTime += shift;                                    // 速算点偏移
            m_baseTime += shift;                                    // 基点时间偏移
        }
    }
    long long clockTime = m_snapTime + tick;                            // 根据速算点算出当前时间
    if (clockTime < m_lastClockTime)                                // 如果时间逆向
    {
        if (m_resumePoint != m_lastClockTime)
        {
            // 避免打很多日志
            m_resumePoint = m_lastClockTime;
            LOG_D("system time slews backward, clockTime[" << clockTime << "] resume point[" << m_lastClockTime << "].");
        }
        
        return m_lastClockTime;                                     // 则返回上个时间值（保持时间值不变）
    }

    if (m_resumePoint)
    {
        m_resumePoint = 0;
        LOG_D("system time [" << clockTime << " exceeds resume point[" << m_lastClockTime << "].");
    }
        
    m_lastClockTime = clockTime;                                    // 记录时间点
    return clockTime;                                               // 返回当前时间
}
/*
long long Clock::getFastSecond()
{
    if (!m_flywheelStart)
    {
        m_nowSecond = getSecond();
        SFW::Instance().start();
        m_flywheelStart = true;
    }
    return m_nowSecond;
}

long long Clock::getFastMillisecond()
{
    if (!m_flywheelStart)
    {
        m_nowMillisecond = getMillisecond();
        SFW::Instance().start();
        m_flywheelStart = true;
    }
    return m_nowMillisecond;
}

long long Clock::getFastMicrosecond()
{
    if (!m_flywheelStart)
    {
        m_nowMicrosecond = getMicrosecond();
        SFW::Instance().start();
        m_flywheelStart = true;
    }
    return m_nowMicrosecond;
}

long long Clock::getFastNanosecond()
{
    if (!m_flywheelStart)
    {
        m_nowNanosecond = getNanosecond();
        SFW::Instance().start();
        m_flywheelStart = true;
    }
    return m_nowNanosecond;
}
*/

time_t Clock::timegm(tm *ptm)
{
    time_t utc;
#ifdef WIN32
    utc = _mkgmtime(ptm);
#elif defined __linux__
    utc = ::timegm(ptm);
#elif defined __sun__
    // Port-able  applications  should  not  try  to create dates before00:00:00 UTC, January 1, 1970 
    // or after 00:00:00 UTC, January 1, 2038.
    // Solaris 10 dose not support ::timegm
    // we do the job on ourselves, as a supplement for Solaris

    // 0:NOT DST  1:DST   -1:DON'T KNOW
    ptm->tm_isdst = 0;

    time_t t1 = mktime(ptm);
    tm tm2;
    //gmtime的线程安全版本
    gmtime_r(&t1, &tm2);

    // 必须保证与ptm一致
    tm2.tm_isdst = 0;

    time_t t2 = mktime(&tm2);
    utc = (t1 + (t1 - t2));
#else
#error Platform required. 
#endif
    return utc;
}

int Clock::gmtime(const time_t *ptime, tm *presult)
{
    int ret = 0;
    //使用平台对应的gmtime的线程安全版本
#ifdef WIN32
    ret = gmtime_s(presult, ptime);
#elif defined __linux__ || defined __sun__
    if (gmtime_r(ptime, presult) == NULL)
    {
        ret = errno;
    }
#else
#error Platform required.
#endif
    return ret;
}

std::string Clock::getUTCString(const long long &nanosecond)
{          
    time_t utc;                                 // UTC数值，单位秒
    tm date;                                    // 年月日时分秒的结构体

    long long second = nanosecond / S;
    try
    {
        utc = lexical_cast <time_t> (second);   // time_t储存的单位为秒
    }
    catch (...)
    {
    	return std::string();
    }
    
    if (Clock::gmtime(&utc, &date) != 0)        // 把时间日期从数值转化成年月日时分秒的结构体
    {
        return std::string();
    }

    char buf[20] = {0};
#ifdef WIN32 
    _snprintf_s(buf, sizeof(buf), 20, "%04d%02d%02dT%02d%02d%02dZ",
        date.tm_year + 1900,
        date.tm_mon + 1,
        date.tm_mday,
        date.tm_hour,
        date.tm_min,
        date.tm_sec);
#elif defined __linux__ || defined __sun__
    snprintf(buf, 20, "%04d%02d%02dT%02d%02d%02dZ",
        date.tm_year + 1900,
        date.tm_mon + 1,
        date.tm_mday,
        date.tm_hour,
        date.tm_min,
        date.tm_sec);
#else
#error Platform required.
#endif

    return std::string(buf);
}

long long Clock::getUTCTime(const std::string &utcstr)
{
    long long nanosecond = -1;
    tm date;
    time_t utc;
    try
    {
        // Port-able  applications  should  not  try  to create dates before00:00:00 UTC, January 1, 1970 
        // or after 00:00:00 UTC, January 1, 2038.
        // now valid range: 19700101T000000Z-20371231T235960Z
        date.tm_year = lexical_cast <int> (utcstr.substr(0, 4)) - 1900;
        if (date.tm_year < 70 || date.tm_year > 137)
        {
            return -1;
        }
        date.tm_mon = lexical_cast <int> (utcstr.substr(4, 2)) - 1;
        if (date.tm_mon < 0 || date.tm_mon > 11)
        {
            return -1;
        }
        date.tm_mday = lexical_cast <int> (utcstr.substr(6, 2));
        if (date.tm_mday < 1 || date.tm_mday > 31)
        {
            return -1;
        }
        date.tm_hour = lexical_cast <int> (utcstr.substr(9, 2));
        if (date.tm_hour < 0 || date.tm_hour > 23)
        {
            return -1;
        }
        date.tm_min = lexical_cast <int> (utcstr.substr(11, 2));
        if (date.tm_min < 0 || date.tm_min > 59)
        {
            return -1;
        }
        date.tm_sec = lexical_cast <int> (utcstr.substr(13, 2));
        if (date.tm_sec < 0 || date.tm_sec > 60)
        {
            return -1;
        }
        date.tm_isdst = -1; // set to -1 means DST is not available, let the system determine whether the TIMEZONE is in DayLightSaving time.

        utc = Clock::timegm(&date);
        if (utc < 0)
        {
            return -1;
        }

        nanosecond = static_cast <long long> (utc) * S;
    }
    catch (...)
    {
    }
    return nanosecond;
}

#ifdef WIN32
long long queryPerformanceFrequency()
{
    long long frequency;
    QueryPerformanceFrequency(reinterpret_cast <LARGE_INTEGER *> (&frequency));
    return frequency;
}
#endif // WIN32

long long Clock::getMonoTick()
{
    static long long lastVal = 0;                                       // 用以检测tick的单向性
#if defined WIN32
    static long long frequency = queryPerformanceFrequency();
    long long performanceCount;
    if (QueryPerformanceCounter(reinterpret_cast <LARGE_INTEGER *> (&performanceCount)) == 0)
        throw GetSystemClockErrorException(getErrorMsg());
    long long ret = performanceCount / frequency * S + performanceCount % frequency * S / frequency;
    if (lastVal < ret)
        lastVal = ret;
    else
        ret = lastVal;
    lastVal = ret;
    return ret;
#elif defined __linux__
    struct timespec tm;
    if (clock_gettime(CLOCK_MONOTONIC, &tm) != 0)
        throw GetSystemClockErrorException(getErrorMsg());
    long long ret = tm.tv_sec * S + tm.tv_nsec;
    if (lastVal < ret)
        lastVal = ret;
    else
        ret = lastVal;
    return ret;
#elif defined __sun__
    struct timespec tm;
    if (clock_gettime(CLOCK_HIGHRES, &tm) != 0)
        throw GetSystemClockErrorException(getErrorMsg());
    long long ret = tm.tv_sec * S + tm.tv_nsec;
    if (lastVal < ret)
        lastVal = ret;
    else
        ret = lastVal;
    return ret;
#else
#error Platform required.
#endif
}

long long Clock::getTime()
{
    #ifdef WIN32
    time_t tmpTime;
    return time(&tmpTime) * S;

    #else
    timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * S + now.tv_usec * US;
    #endif
}

}
}
}
