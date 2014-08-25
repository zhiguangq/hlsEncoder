/**
 * Title: 
 * Description: Define template functions to simplify logging.
 * Copyright: Copyright (c) 2013 ocn (All rights reserved)
 * Company: ocn
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-9-24
 * Version: 1.0
 * History: [1.0 2013-9-24]
 */

#ifndef LOG_FACILITY_BASE_OCN_H
#define LOG_FACILITY_BASE_OCN_H

#include <string>
#include <typeinfo>
#include <boost/foreach.hpp>
//#include <log4cxx/logger.h>
#include <glog/logging.h>
#include <ocn/base/facility/Demangle.h>


namespace ocn
{
namespace base
{
namespace facility
{
#define LOG_T(msg) LOG(INFO) << msg 
#define LOG_D(msg) LOG(INFO) << msg
#define LOG_I(msg) LOG(INFO) << msg
#define LOG_W(msg) LOG(WARNING) << msg
#define LOG_E(msg) LOG(ERROR) << msg
#define LOG_F(msg) LOG(FATAL) << msg

#define LOG_R_T(msg) LOG(INFO) << msg
#define LOG_R_D(msg) LOG(INFO) << msg
#define LOG_R_I(msg) LOG(INFO) << msg
#define LOG_R_W(msg) LOG(WARNING) << msg
#define LOG_R_E(msg) LOG(ERROR) << msg
#define LOG_R_F(msg) LOG(FATAL) << msg
/*
std::string colon2dot(const std::string& name);

std::string demangling(const std::string& name);


template<typename T> log4cxx::LoggerPtr getLoggerByClass(T* p)
{
	static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(colon2dot(demangling(typeid(*p).name())));
	return logger;
}


#define LOG_T(msg) LOG4CXX_TRACE(ocn::base::facility::getLoggerByClass(this), msg)
#define LOG_D(msg) LOG4CXX_DEBUG(ocn::base::facility::getLoggerByClass(this), msg)
#define LOG_I(msg) LOG4CXX_INFO (ocn::base::facility::getLoggerByClass(this), msg)
#define LOG_W(msg) LOG4CXX_WARN (ocn::base::facility::getLoggerByClass(this), msg)
#define LOG_E(msg) LOG4CXX_ERROR(ocn::base::facility::getLoggerByClass(this), msg)
#define LOG_F(msg) LOG4CXX_FATAL(ocn::base::facility::getLoggerByClass(this), msg)

inline log4cxx::LoggerPtr getRootLogger()
{
	static log4cxx::LoggerPtr rootLogger = log4cxx::Logger::getRootLogger();
	return rootLogger;
}

#define LOG_R_T(msg) LOG4CXX_TRACE(ocn::base::facility::getRootLogger(), msg)
#define LOG_R_D(msg) LOG4CXX_DEBUG(ocn::base::facility::getRootLogger(), msg)
#define LOG_R_I(msg) LOG4CXX_INFO (ocn::base::facility::getRootLogger(), msg)
#define LOG_R_W(msg) LOG4CXX_WARN (ocn::base::facility::getRootLogger(), msg)
#define LOG_R_E(msg) LOG4CXX_ERROR(ocn::base::facility::getRootLogger(), msg)
#define LOG_R_F(msg) LOG4CXX_FATAL(ocn::base::facility::getRootLogger(), msg)
*/
}
}
}

#endif // 
