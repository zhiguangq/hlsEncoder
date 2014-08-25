#include "TaskQueue.h"


TaskQueue::TaskQueue(void)
{
}


TaskQueue::~TaskQueue(void)
{
}

bool TaskQueue::producer(std::string xmlFile, std::string tsFile)
{
    boost::mutex::scoped_lock lk(m_mutexStack);
    boost::shared_ptr<TackNode> node(new TackNode(xmlFile, tsFile));
    m_stack.push(node);

    return true;
}

bool TaskQueue::empty(void)
{
    return m_stack.empty();
}

bool TaskQueue::consumer(std::string& xmlFile, std::string& tsFile)
{
    if(!m_stack.empty())
    {
        boost::shared_ptr<TackNode> node = m_stack.top();
        m_stack.pop();
        xmlFile = node->m_xmlFile;
        tsFile = node->m_tsFile;
        return true;
    }
    return false;
}
