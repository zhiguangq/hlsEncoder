/**
 * Title: HTTP Connection Manager
 * Description: 管理所有的HTTP connection
 * Copyright: Copyright (c) 2013 ocn (All rights reserved)
 * Company: ocn
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-9-24
 * Version: 1.0
 * History: [1.0 2013-9-24]
 */

#ifndef HTTPCONNECTIONMANAGER_NET_BASE_OCN_H
#define HTTPCONNECTIONMANAGER_NET_BASE_OCN_H

#include <set>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include <ocn/base/net/HTTPConnection.h>

namespace ocn
{
namespace base
{
namespace net
{

class HTTPConnectionManager
    :private boost::noncopyable
{
public:
    HTTPConnectionManager();
    /// Add the specified connection to the manager and start it.
    void start(ConnectionSharePtr c);

    /// Stop the specified connection.
    void stop(ConnectionSharePtr c);

    /// Stop all connections.
    void stopAll();

    void registerHandler(Handler* handler)
    {
        m_requestHandler->registerHandler(handler);
    }

    void newRequest(void* req, std::string uri, std::string content);
private:
    /// The managed connections.
    std::set<ConnectionSharePtr>                m_connections;
    boost::scoped_ptr<HTTPRequestHandler>       m_requestHandler;
};

} // http    
} // server
} // ocn

#endif  // HTTPCONNECTIONMANAGER_NET_BASE_OCN_H
