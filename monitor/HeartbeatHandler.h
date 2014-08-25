#ifndef HEARTBEATHANDLER_H
#define HEARTBEATHANDLER_H

#include <string>

#include <kugou/base/net/ImplHandler.h>
#include "HeartbeatRequest.h"
#include "HeartbeatResponse.h"


class HeartbeatHandler : public kugou::base::net::ImplHandler<HeartbeatRequest,HeartbeatResponse>
{
public:
    HeartbeatHandler(void)
    {
    }
	std::string name()
	{
		return HEARTBEAT_API;
	}
    HeartbeatHandler* clone(void);
protected:
    virtual void getPeerAddress(const std::string& peerAddress);
	virtual bool impl_handle();
	virtual bool impl_check();

private:
    std::string     m_peerAddress;
};


#endif
