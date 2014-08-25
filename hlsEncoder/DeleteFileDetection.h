#ifndef DELETEFILEDETECTION_H
#define DELETEFILEDETECTION_H

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/unordered_set.hpp>


class DeleteFileDetection
{
public:
    DeleteFileDetection(void);
    ~DeleteFileDetection(void);

    void start(void);
private:
    void run(void);
    boost::scoped_ptr<boost::thread>    m_thread;
};

#endif
