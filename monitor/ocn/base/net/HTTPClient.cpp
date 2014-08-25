#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include <ocn/base/net/HTTPClient.h>
#include <ocn/base/facility/log.h>

namespace ocn
{
namespace base
{
namespace net
{

HTTPClient::HTTPClient(const std::string& ip, const std::string& port, const std::string& uri)
    : m_ioService()
    , m_resolver(m_ioService)
    , m_ip(ip)
    , m_port(port)
    , m_uri(uri)
{
}

bool HTTPClient::HTTPGet(std::string domin)
{
    try
    {
        boost::asio::ip::tcp::resolver::query query(domin,"http");

        boost::asio::ip::tcp::resolver::iterator endpoint_iterator = m_resolver.resolve(query);
        boost::asio::ip::tcp::resolver::iterator end;

        //定义socket并且连接
	    boost::asio::ip::tcp::socket socket(m_ioService);
	    boost::system::error_code error = boost::asio::error::host_not_found;
	    while (error && endpoint_iterator != end)
	    {
		    socket.close();
		    socket.connect(*endpoint_iterator++, error);
	    }
	    if (error)
		    throw boost::system::system_error(error);

        // 构造request
		boost::asio::streambuf request;
		std::ostream request_stream(&request);
		request_stream << "GET " << m_uri << " HTTP/1.1\r\n";
        request_stream << "host: " << domin << "\r\n";
        request_stream << "Connection:close\r\n";
        request_stream << "\r\n";

        //发送查询命令
        boost::asio::write(socket, request);

        //读取查询结果
        boost::asio::streambuf response;

        //读取http的文件头，然后返回
        boost::asio::read_until(socket, response, "\r\n");

        //获得http状态码
        std::istream response_stream(&response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			LOG_W("Invalid response\n");
			return false;
		}
		if (status_code != 200)
		{
			LOG_W("Response returned with status code " << status_code << "\n");
			return false;
		}
        // 不处理回复其它的东西了
    }
	catch (std::exception& e)
	{
		LOG_W("Exception: " << e.what() << "\n");
        return false;
	}
    return true;
}

bool HTTPClient::HTTPGet(std::string& requestJson, std::string& responseJson)
{
    try
    {
        boost::asio::ip::tcp::resolver::query query(m_ip, m_port);

        boost::asio::ip::tcp::resolver::iterator endpoint_iterator = m_resolver.resolve(query);
        boost::asio::ip::tcp::resolver::iterator end;

        //定义socket并且连接
	    boost::asio::ip::tcp::socket socket(m_ioService);
	    boost::system::error_code error = boost::asio::error::host_not_found;
	    while (error && endpoint_iterator != end)
	    {
		    socket.close();
		    socket.connect(*endpoint_iterator++, error);
	    }
	    if (error)
		    throw boost::system::system_error(error);

        // 构造request
		boost::asio::streambuf request;
		std::ostream request_stream(&request);
		request_stream << "POST " << m_uri << " HTTP/1.1\r\n";
        request_stream << "Content-Length: " << requestJson.length() << "\r\n";
        request_stream << "Content-Type: text/plain\r\n\r\n";
        request_stream << requestJson;

        //发送查询命令
        boost::asio::write(socket, request);

        //读取查询结果
        boost::asio::streambuf response;

        //读取http的文件头，然后返回
        boost::asio::read_until(socket, response, "\r\n");

        //获得http状态码
        std::istream response_stream(&response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			LOG_W("Invalid response\n");
			return false;
		}
		if (status_code != 200)
		{
			LOG_W("Response returned with status code " << status_code << "\n");
			return false;
		}

		//读取http头
		boost::asio::read_until(socket, response, "\r\n\r\n");

		// Process the response headers.
		std::string header;
        int contentLength = 0;
		while (std::getline(response_stream, header) && header != "\r")
        {
            if(header.find("Content-Length") != std::string::npos \
                || header.find("Content-length") != std::string::npos)
            {
                std::size_t posBegin = header.find_first_of("123456789");
                std::size_t posEnd = header.find_last_of("1234567890");
                std::string lengthStr = header.substr(posBegin, (posEnd - posBegin + 1));
                contentLength = boost::lexical_cast<int> (lengthStr);
            }

			//LOG_D(header);
        }

		// Write whatever content we already have to output.
        char buf[4*1024];
        int totalSize = 0;
		while (response.size() > 0 || boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
        {
            int size = response.size();            
            response_stream.read(buf + totalSize, size);   
            totalSize += size;
            if(totalSize >= contentLength)
            {
                // 收到的content内容
                break;
            }
        }

        buf[contentLength] = '\0';
        responseJson.append(buf);
    }
	catch (std::exception& e)
	{
		LOG_W("Exception: " << e.what() << "\n");
        return false;
	}

    return true;
}


} // http    
} // server
} // ocn
