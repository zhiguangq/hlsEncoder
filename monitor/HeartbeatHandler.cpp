#include <iostream>
#include "HeartbeatHandler.h"
#include "EncoderManager.h"


bool HeartbeatHandler::impl_check()
{
	return true;
}

HeartbeatHandler* HeartbeatHandler::clone(void)
{
    return new HeartbeatHandler(*this);
}

void HeartbeatHandler::getPeerAddress(const std::string& peerAddress)
{
    m_peerAddress = peerAddress;
}

bool HeartbeatHandler::impl_handle()
{
    EncoderManager::instance().update(m_peerAddress);

    return true;
}

