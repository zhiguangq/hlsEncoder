#ifndef STREAMSOCKET_NET_BASE_OCN_H
#define STREAMSOCKET_NET_BASE_OCN_H

#include <string>

#if defined WIN32
//#define FD_SETSIZE 2048
#include <winsock2.h>
#elif defined __linux__ || defined __sun__
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <fcntl.h>
#include <arpa/inet.h>
#else
#error Platform required.
#endif

#if defined WIN32
#define INADDR_INT(addr) addr.S_un.S_addr
typedef unsigned short sa_family_t;
#elif defined __linux__ || defined __sun__
#define SOCKET int
#define SD_RECEIVE 0x00
#define SD_SEND 0x01
#define SD_BOTH 0x02
#define INVALID_SOCKET (SOCKET)(~0)
#define INADDR_INT(addr) addr.s_addr
#else
#error Platform required.
#endif

namespace ocn
{
namespace base
{
namespace net
{

class StreamSocket
{
public:
    StreamSocket(std::string ip, int port, int timeout = -1, int bufferSize = 1024 * 64);
    ~StreamSocket();

    int getStreamSocket(void);
protected:
	void bind(void);
private:
	std::string 	m_ip;
	int				m_port;
    int     		m_s;
};

}
}
}

#endif // STREAMSOCKET_NET_BASE_OCN_H
