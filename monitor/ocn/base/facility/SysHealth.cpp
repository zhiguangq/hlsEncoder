#include <ocn/base/facility/SysHealth.h>

#ifdef _WIN32_WINDOWS
#include <windows.h>
#else
#include <fstream>
#include <string>
#endif

using namespace std;
namespace ocn
{
namespace base
{
namespace facility
{

SysHealth::SysHealthPtr SysHealth::m_sysHealthPtr;
boost::mutex      SysHealth::m_sysHealthMutex;

SysHealth::SysHealthPtr SysHealth::Instance(void)
{
    try
    {
        boost::mutex::scoped_lock lckAssgn(m_sysHealthMutex);

        if (m_sysHealthPtr.get() == NULL)
        {
            m_sysHealthPtr = SysHealthPtr(new SysHealth);
        }
    }
    catch(const std::bad_alloc &KE)
    {
        //LOG_E(KE.what());
        return SysHealthPtr();
    }
    catch(...)
    {
        //LOG_E("Failed to get SysHealth instance with unknown error");
        return SysHealthPtr();
    }

    return m_sysHealthPtr;
}

const long long SysHealth::getPidMemory(unsigned int pid)
{
#ifdef _WIN32_WINDOWS
	return 0;
#else
    char statmPath[128];
    sprintf(statmPath,"/proc/%d/statm", pid);
    ifstream m_pidMemoryFd;
    m_pidMemoryFd.open(statmPath);
    if(!m_pidMemoryFd)
    {
        cerr << "open pid meminfo error." << endl;
        return -1;
    }

    m_pidMemoryFd.seekg(0, ios::beg);
    string ph;
    long long pidMemoryTotal;

    // pid mem is locate on the second item in line
    m_pidMemoryFd >> ph >> pidMemoryTotal >> ph >> ph >> ph;

    // pid mem is 4k per, so multiply 4 again 
    pidMemoryTotal <<=2;

    m_pidMemoryFd.close();
    return pidMemoryTotal;
#endif
}
	
const long long SysHealth::getTotalMemory(void)
{
#ifdef _WIN32_WINDOWS
	return 0;
#else
    string meminfo = "/proc/meminfo";
    ifstream m_meminfoFd;
    m_meminfoFd.open(meminfo.c_str());
    if(!m_meminfoFd)
    {
        cerr << "open meminfo error." << endl;
        return -1;
    }

    m_meminfoFd.seekg(0, ios::beg);
    string ph;
    long long memTotal;

    // total mem is locate on the second item in line
    m_meminfoFd >> ph >> memTotal >> ph >> ph >> ph;

    //memTotal <<= 10;
    m_meminfoFd.close();
    return memTotal;
#endif
}

const int SysHealth::cpuUsage(void) const
{
    boost::mutex::scoped_lock lockCpuUsage(m_cpuUsageMutex);

#ifdef _WIN32_WINDOWS
    FILETIME               ft_sys_idle;
    FILETIME               ft_sys_kernel;
    FILETIME               ft_sys_user;

    ULARGE_INTEGER         ul_sys_idle;
    ULARGE_INTEGER         ul_sys_kernel;
    ULARGE_INTEGER         ul_sys_user;

    static ULARGE_INTEGER	 ul_sys_idle_old;
    static ULARGE_INTEGER  ul_sys_kernel_old;
    static ULARGE_INTEGER  ul_sys_user_old;

    CHAR  usage = 0;

    GetSystemTimes(&ft_sys_idle,    //System idle time 
                 &ft_sys_kernel,    // system kernel time
                 &ft_sys_user);     // System user time 

    CopyMemory(&ul_sys_idle  , &ft_sys_idle  , sizeof(FILETIME)); // Could been optimized away...
    CopyMemory(&ul_sys_kernel, &ft_sys_kernel, sizeof(FILETIME)); // Could been optimized away...
    CopyMemory(&ul_sys_user  , &ft_sys_user  , sizeof(FILETIME)); // Could been optimized away...

    
    usage  =
            (
            (
            (
                (
                (ul_sys_kernel.QuadPart - ul_sys_kernel_old.QuadPart)+
                (ul_sys_user.QuadPart   - ul_sys_user_old.QuadPart)
                )
                -
                (ul_sys_idle.QuadPart-ul_sys_idle_old.QuadPart)
            )
            *
            (100)
            )
            /
            (
            (ul_sys_kernel.QuadPart - ul_sys_kernel_old.QuadPart)+
            (ul_sys_user.QuadPart   - ul_sys_user_old.QuadPart)
            )
            );          

    ul_sys_idle_old.QuadPart   = ul_sys_idle.QuadPart;
    ul_sys_user_old.QuadPart   = ul_sys_user.QuadPart;
    ul_sys_kernel_old.QuadPart = ul_sys_kernel.QuadPart;

    return (int)usage;
#else
    static long long m_oldUser = 0;
    static long long m_oldNice = 0;
    static long long m_oldSys = 0;
    static long long m_oldIdle = 0;

    m_stat.seekg(0, std::ios::beg);
    std::string ph;
    long long user, nice, sys, idle;
    m_stat >> ph >> user >> nice >> sys >> idle;

    long long util = (user - m_oldUser) + (nice - m_oldNice) + (sys - m_oldSys);
    long long a = util + (idle - m_oldIdle);
    if (a <= 0)
    {
        a = 1;
    }
    m_oldUser = user;
    m_oldNice = nice;
    m_oldSys = sys;
    m_oldIdle = idle;

    return util * 100 / a;
#endif
}

SysHealth::SysHealth(void)
{
#ifdef _WIN32_WINDOWS
#else
            m_stat.open("/proc/stat");
            if (!m_stat)
            {
                throw "open /proc/stat error";
            }
#endif
}


SysHealth::~SysHealth(void)
{
}

}
}
}
