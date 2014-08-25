#include <iostream>
#include "MessageHandler.h"
#include "EncoderManager.h"


bool MessageHandler::impl_check()
{
	return true;
}

MessageHandler* MessageHandler::clone(void)
{
    return new MessageHandler(*this);
}

void MessageHandler::getPeerAddress(const std::string& peerAddress)
{
    m_peerAddress = peerAddress;
}

bool MessageHandler::impl_handle()
{
    EncoderManager::instance().message(m_peerAddress,this->m_request.Message);

    return true;
}

