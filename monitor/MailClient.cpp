#include "MailClient.h"

#include <iostream>

#include <Poco/Net/MailMessage.h>
#include <Poco/Net/MailRecipient.h>
#include <Poco/Net/SMTPClientSession.h>
#include <Poco/Net/NetException.h>

using namespace std;
using namespace Poco::Net;
using namespace Poco;

MailClient::MailClient(void)
{
}


MailClient::~MailClient(void)
{
}

bool MailClient::Send(std::string& content)
{
    // qq gmail 不支持非SSL
	string host = "211.167.97.252";
    UInt16 port = 25;
	string user = "xgxx@scn.com.cn";
	string password = "kvM62gK2";
	string to = "maintain@dawnsong.cc";
	string from = user;
    
    string subject = "Enocder System Info";
    subject = MailMessage::encodeWord(subject, "UTF-8");    
   
    MailMessage message;
    message.setSender(from);
    message.addRecipient(MailRecipient(MailRecipient::PRIMARY_RECIPIENT, to));
    message.setSubject(subject);
    message.setContentType("text/plain; charset=UTF-8");
    message.setContent(content, MailMessage::ENCODING_8BIT);
    
    try 
    {
        SMTPClientSession session(host, port);
        session.open();
        try
        {
            session.login(SMTPClientSession::AUTH_LOGIN, user, password);
            session.sendMessage(message);
            //cout << "Message successfully sent" << endl;
            session.close();
        } 
        catch (SMTPException &e) 
        {
            cerr << e.displayText() << endl;
            session.close();
            return false;
        }
    } 
    catch (NetException &e) 
    {
        cerr << e.displayText() << endl;
        return false;
    }

    return true;
}
