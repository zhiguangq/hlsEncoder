#ifndef FILEDETECTION_H
#define FILEDETECTION_H

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/unordered_set.hpp>
#include <Poco/Timestamp.h>

#define SCAN_INTERVAL   15 * 60 // 以秒为单位

class FileDetection
{
    typedef boost::unordered_set<std::string>   StringSet;
public:
    FileDetection(std::string path);
    ~FileDetection(void);

    void start(void);
private:
    void run(void);

    std::string                         m_scanPath;
    boost::shared_ptr<StringSet>        m_allFileSet;
    boost::scoped_ptr<boost::thread>    m_thread;
    Poco::Timestamp                     m_lastXMLGeneraateTime;
};

#endif
