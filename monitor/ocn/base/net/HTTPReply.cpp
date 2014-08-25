#include <boost/lexical_cast.hpp>

#include <ocn/base/net/HTTPReply.h>

using namespace boost::asio;

namespace ocn
{
namespace base
{
namespace net
{

const std::string m_ok = "HTTP/1.0 200 OK\r\n";
const std::string m_created = "HTTP/1.0 201 Created\r\n";
const std::string m_accepted = "HTTP/1.0 202 Accepted\r\n";
const std::string m_no_content = "HTTP/1.0 204 No Content\r\n";
const std::string m_multiple_choices = "HTTP/1.0 300 Multiple Choices\r\n";
const std::string m_moved_permanently = "HTTP/1.0 301 Moved Permanently\r\n";
const std::string m_moved_temporarily = "HTTP/1.0 302 Moved Temporarily\r\n";
const std::string m_not_modified = "HTTP/1.0 304 Not Modified\r\n";
const std::string m_bad_request = "HTTP/1.0 400 Bad Request\r\n";
const std::string m_unauthorized = "HTTP/1.0 401 Unauthorized\r\n";
const std::string m_forbidden = "HTTP/1.0 403 Forbidden\r\n";
const std::string m_not_found = "HTTP/1.0 404 Not Found\r\n";
const std::string m_internal_server_error = "HTTP/1.0 500 Internal Server Error\r\n";
const std::string m_not_implemented = "HTTP/1.0 501 Not Implemented\r\n";
const std::string m_bad_gateway = "HTTP/1.0 502 Bad Gateway\r\n";
const std::string m_service_unavailable = "HTTP/1.0 503 Service Unavailable\r\n";
const char m_name_value_separator[] = { ':', ' '};
const char m_crlf[] = { '\r', '\n' };

const_buffer HTTPReply::statusToString(status_type status)
{
    switch (status)
    {
    case HTTPReply::ok:
        return boost::asio::buffer(m_ok);
    case HTTPReply::created:
        return boost::asio::buffer(m_created);
    case HTTPReply::accepted:
        return boost::asio::buffer(m_accepted);
    case HTTPReply::no_content:
        return boost::asio::buffer(m_no_content);
    case HTTPReply::multiple_choices:
        return boost::asio::buffer(m_multiple_choices);
    case HTTPReply::moved_permanently:
        return boost::asio::buffer(m_moved_permanently);
    case HTTPReply::moved_temporarily:
        return boost::asio::buffer(m_moved_temporarily);
    case HTTPReply::not_modified:
        return boost::asio::buffer(m_not_modified);
    case HTTPReply::bad_request:
        return boost::asio::buffer(m_bad_request);
    case HTTPReply::unauthorized:
        return boost::asio::buffer(m_unauthorized);
    case HTTPReply::forbidden:
        return boost::asio::buffer(m_forbidden);
    case HTTPReply::not_found:
        return boost::asio::buffer(m_not_found);
    case HTTPReply::internal_server_error:
        return boost::asio::buffer(m_internal_server_error);
    case HTTPReply::not_implemented:
        return boost::asio::buffer(m_not_implemented);
    case HTTPReply::bad_gateway:
        return boost::asio::buffer(m_bad_gateway);
    case HTTPReply::service_unavailable:
        return boost::asio::buffer(m_service_unavailable);
    default:
        return boost::asio::buffer(m_internal_server_error);
    }
}

HTTPReply::ConstBufferVector HTTPReply::ReplyBuffers()
{
    HTTPReply::ConstBufferVector buffers;
    buffers.push_back(statusToString(status));
    for(std::size_t i = 0; i < m_headers.size(); ++i)
    {
        HTTPHeader& h = m_headers[i];
        buffers.push_back(boost::asio::buffer(h.m_name));
        buffers.push_back(boost::asio::buffer(m_name_value_separator));
        buffers.push_back(boost::asio::buffer(h.m_value));
        buffers.push_back(boost::asio::buffer(m_crlf));
    }

    buffers.push_back(boost::asio::buffer(m_crlf));
    buffers.push_back(boost::asio::buffer(m_content));

    return buffers;
}

    
} // http    
} // server
} // ocn
