/**
 * Title: HTTP Client
 * Description: HTTP Client
 * Copyright: Copyright (c) 2013 ocn (All rights reserved)
 * Company: ocn
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-10-23
 * Version: 1.0
 * History: [1.0 2013-10-23]
 */

#ifndef HTTPCLIENT_HTTP_SERVER_OCN_H
#define HTTPCLIENT_HTTP_SERVER_OCN_H

//#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace ocn
{
namespace base
{
namespace net
{

class HTTPClient
    : private boost::noncopyable
{
public:
    /// Construct the server to listen on the specified TCP address and port, and
    /// server up file from the given directory.
    explicit HTTPClient(const std::string& ip, const std::string& port, const std::string& uri);

    /// Run the server's io_service loop.
    bool HTTPGet(std::string& request, std::string& response);

    bool HTTPGet(std::string domin);

private:
    boost::asio::io_service             m_ioService;
    boost::asio::ip::tcp::resolver      m_resolver;

    std::string                         m_ip;
    std::string                         m_port;
    std::string                         m_uri;
};

} // http    
} // server
} // ocn

#endif
