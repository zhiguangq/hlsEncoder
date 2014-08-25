/**
 * Title: monitor system health 
 * Description: get cpu usage
 * Copyright: Copyright (c) 2013 ocn (All rights reserved)
 * Company: ocn
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-10-21
 * Version: 1.0
 * History: [1.0 2013-10-21]
 */

#ifndef SYSTEMHEALTH_FACILITY_BASE_OCN_H
#define SYSTEMHEALTH_FACILITY_BASE_OCN_H

#include <iostream>
#include <memory>
#include <string>
#include <fstream>

#include <boost/thread/mutex.hpp>

#define DISALLOW_COPY(ClassName) \
    ClassName(const ClassName &)
#define DISALLOW_ASSIGN(ClassName) \
    ClassName &operator = (const ClassName &)
#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
    DISALLOW_COPY(ClassName);               \
    DISALLOW_ASSIGN(ClassName)               // no semicolon here

namespace ocn
{
namespace base
{
namespace facility
{

class SysHealth
{
public:
    typedef boost::shared_ptr<SysHealth> SysHealthPtr;
    static SysHealthPtr Instance(void);

    const int cpuUsage(void) const;
	
	// return : 返回单位为 K
	const long long getPidMemory(unsigned int pid);
	
	// return : 返回单位为 K
	const long long getTotalMemory(void);

    ~SysHealth(void);
private:
    mutable boost::mutex        m_cpuUsageMutex;
    mutable std::ifstream       m_stat;

    static SysHealthPtr         m_sysHealthPtr;
    static boost::mutex         m_sysHealthMutex;

    SysHealth(void);
    DISALLOW_COPY_AND_ASSIGN(SysHealth);
};

}
}
}
#endif
