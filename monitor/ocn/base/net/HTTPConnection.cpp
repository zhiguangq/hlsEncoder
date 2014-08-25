#include <vector>

#include <boost/bind.hpp>
#include <boost/logic/tribool.hpp>

#include <ocn/base/net/HTTPConnection.h>
#include <ocn/base/net/HTTPConnectionManager.h>

namespace ocn
{
namespace base
{
namespace net
{

HTTPConnection::HTTPConnection(boost::asio::io_service &io_service, HTTPConnectionManager &manager, HTTPRequestHandler &handler)
    : m_socket(io_service)
    , m_connectionManager(manager)
    , m_requestHandler(handler)
{
}

boost::asio::ip::tcp::socket& HTTPConnection::socket()
{
    return m_socket;
}

void HTTPConnection::start()
{
    m_socket.async_read_some(boost::asio::buffer(m_buffer),
        boost::bind(&HTTPConnection::handleRead, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void HTTPConnection::stop()
{
    m_socket.close();
}

void HTTPConnection::handleRead(const boost::system::error_code &e, std::size_t bytes_transferrd)
{
}

void HTTPConnection::handleWrite(const boost::system::error_code& e)
{
    if(!e)
    {
        // Initiate graceful connection closure.
        boost::system::error_code ignored_ec;
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    }

    if(e != boost::asio::error::operation_aborted)
    {
        m_connectionManager.stop(shared_from_this());
    }
}

} // http    
} // base
} // net
