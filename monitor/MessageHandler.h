#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <string>

#include <ocn/base/net/ImplHandler.h>
#include "MessageRequest.h"
#include "MessageResponse.h"


class MessageHandler : public ocn::base::net::ImplHandler<MessageRequest,MessageResponse>
{
public:
    MessageHandler(void)
    {
    }
	std::string name()
	{
		return MESSAGE_API;
	}
    MessageHandler* clone(void);
protected:
    virtual void getPeerAddress(const std::string& peerAddress);
	virtual bool impl_handle();
	virtual bool impl_check();

private:
    std::string     m_peerAddress;
};


#endif
