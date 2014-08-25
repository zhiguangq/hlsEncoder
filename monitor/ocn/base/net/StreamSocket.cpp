#include <ocn/base/net/StreamSocket.h>
#include <ocn/base/facility/ErrorMsg.h>
#include <ocn/base/facility/Exception.h>

#include <iostream>

using namespace ocn::base::facility;

namespace ocn
{
namespace base
{
namespace net
{
EXCEPTION(SocketException, ocn::base::facility::Exception);      // SocketException
EXCEPTION(BindFailureException, ocn::base::facility::Exception);   // BindFailureException
EXCEPTION(FCNTLException, ocn::base::facility::Exception);      // FCNTLException

StreamSocket::StreamSocket(std::string ip, int port, int timeout, int bufferSize)
	: m_ip(ip)
	, m_port(port)
{
	this->bind();
}

void StreamSocket::bind(void)
{
    m_s = socket(AF_INET, SOCK_STREAM, 0);
    if (m_s == INVALID_SOCKET)
    {
        throw SocketException(getErrorMsg());
    }	
	
	int one = 1;
	setsockopt(m_s, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(int));

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(m_ip.c_str());//INADDR_ANY;
	addr.sin_port = htons(m_port);
	
	if(::bind(m_s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		throw BindFailureException(getErrorMsg());
	}

	::listen(m_s, 10240);
	
    // 把socket设置成非阻塞的
#if defined WIN32
    u_long flag = 1;
    if(ioctlsocket(m_s, FIONBIO, &flag) < 0)
#elif defined __sun__ || defined __linux__
    int flags;
	if ((flags = fcntl(m_s, F_GETFL, 0)) < 0 || fcntl(m_s, F_SETFL, flags | O_NONBLOCK) < 0)
#endif
	{
		throw FCNTLException(getErrorMsg());
	}
}

int StreamSocket::getStreamSocket(void)
{
    return m_s;
}

StreamSocket::~StreamSocket()
{

}

}
}
}

