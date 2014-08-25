#ifndef MAILCLIENT_H
#define MAILCLIENT_H
#include <iostream>

class MailClient
{
public:
    MailClient(void);
    bool Send(std::string& content);
    ~MailClient(void);
};

#endif
