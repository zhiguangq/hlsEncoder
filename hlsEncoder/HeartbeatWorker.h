#ifndef HEARTBEATWORKER_H
#define HEARTBEATWORKER_H

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/unordered_set.hpp>

class HeartbeatWorker
{
public:
    HeartbeatWorker(void);
    ~HeartbeatWorker(void);
    void start(void);
private:
    void run(void);

    boost::scoped_ptr<boost::thread>    m_thread;
};

#endif
