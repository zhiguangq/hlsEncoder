#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include "Singleton.h"
//#include <boost/lockfree/queue.hpp>
//#include <boost/lockfree/spsc_queue.hpp>
#include <stack>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

class TaskQueue : public Singleton<TaskQueue>
{
    friend class Singleton<TaskQueue>;
private:
    class TackNode
    {
    public:
        TackNode(std::string& xmlFile, std::string& tsFile)
        {
            m_xmlFile = xmlFile;
            m_tsFile = tsFile;
        }
        std::string m_xmlFile;
        std::string m_tsFile;
    };
public:
    ~TaskQueue(void);

    bool producer(std::string xmlFile, std::string tsFile);
    bool consumer(std::string& xmlFile, std::string& tsFile);
    bool empty(void);
private:
    TaskQueue(void);

    // boost::lockfree::spsc_queue 用于单线程读，单线程写的场景
    // boost::lockfree::queue 可以用于多线程读，多线程写的场景
    std::stack<boost::shared_ptr<TackNode>>          m_stack;         // 用堆栈的原因是优先让新生成的TS文件先编码
    boost::mutex                                    m_mutexStack;
};

#endif
