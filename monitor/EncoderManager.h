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
#include <ocn/base/facility/Json.h>

struct ActiveEncoder : public ocn::base::facility::Json
{
    std::vector<std::string>    EncoderAddress;

private:
	void assign()
	{
        JSON_ADD_OBJECT_STRING(EncoderAddress);
	}
};

class EncoderManager : public ocn::base::facility::Singleton<EncoderManager>
{
public:
    EncoderManager(void);
    ~EncoderManager(void);

    void update(const std::string& address);
    void message(const std::string& address, std::string& mess);

    void setExePath(std::string& path);
    void start(void);
private:
    void run(void);

    boost::scoped_ptr<boost::thread>    m_thread;

    typedef std::map<std::string, Poco::Timestamp> EncoderMap;
    EncoderMap      m_encoderMap;
    boost::mutex    m_mutexEncoderMap;
    std::string     m_exePath;
};

#endif
