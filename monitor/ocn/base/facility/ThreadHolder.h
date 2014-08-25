#ifndef THREADHOLDER_FACILITY_BASE_OCN_H
#define THREADHOLDER_FACILITY_BASE_OCN_H

#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>

namespace ocn
{
namespace base
{
namespace facility
{

template <typename T> class ThreadHolder
{
public:
    ThreadHolder() : m_t()
    {
    }

    template <typename P> ThreadHolder(P p) : m_t(p)
    {
    }

    template <typename P1, typename P2> ThreadHolder(P1 p1, P2 p2) : m_t(p1, p2)
    {
    }

    template <typename P1, typename P2, typename P3> ThreadHolder(P1 p1, P2 p2, P3 p3) : m_t(p1, p2, p3)
    {
    }

    template <typename P1, typename P2, typename P3, typename P4> ThreadHolder(P1 p1, P2 p2, P3 p3, P4 p4) : m_t(p1, p2, p3, p4)
    {
    }

    template <typename P1, typename P2, typename P3, typename P4, typename P5> ThreadHolder(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) : m_t(p1, p2, p3, p4, p5)
    {
    }

    ~ThreadHolder()
    {
        m_t.setRunning(false);
        if (m_thread)
        {
            m_thread->join();
            m_thread.reset();
        }
    }

    bool start()
    {
        boost::mutex::scoped_lock lk(m_mutex);
        if (!m_t.running())
        {
            m_t.setRunning(true);
            m_thread.reset(new boost::thread(boost::ref(*this)));
            return true;
        }
        else
        {
            return false;
        }
    }

    void stop()
    {
        m_t.setRunning(false);
    }

    void join()
    {
        if (m_thread)
        {
            m_thread->join();
            m_thread.reset();
        }
    }

    void operator () ()
    {
        // Thread started.
        m_t.run();
        // Thread stopped.
        assert(!m_t.running());
    }

    T *operator -> ()
    {
        return &m_t;
    }

    T &operator * ()
    {
        return m_t;
    }

private:
    // To ensure that instance of T overlives the corresponding m_thread object
    T m_t;
    boost::mutex m_mutex;
    boost::scoped_ptr <boost::thread> m_thread;
};

template <class T> struct CreateAndStart
{
    static T *Create()
    {
        T *t = new T;
        t->start();
        return t;
    }

    static void Destroy(T *p)
    {
        delete p;
    }
};

}
}
}

#endif // THREADHOLDER_FACILITY_BASE_OCN_H
