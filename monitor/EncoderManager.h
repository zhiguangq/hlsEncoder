#ifndef HEARTBEATDETECTION_H
#define HEARTBEATDETECTION_H

#include <string>
#include <map>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/unordered_set.hpp>

#include <ocn/base/facility/Singleton.h>
#include <Poco/Timestamp.h>

class EncoderManager : public ocn::base::facility::Singleton<EncoderManager>
{
public:
    EncoderManager(void);
    ~EncoderManager(void);

    void update(const std::string& address);

    void start(void);
private:
    void run(void);

    boost::scoped_ptr<boost::thread>    m_thread;

    typedef std::map<std::string, Poco::Timestamp> EncoderMap;
    EncoderMap      m_encoderMap;
    boost::mutex    m_mutexEncoderMap;
};

#endif
