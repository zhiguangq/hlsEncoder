#ifndef MESSAGERESPONSE_H
#define MESSAGERESPONSE_H

#include <string>
#include <vector>
#include <ocn/base/facility/Json.h>

struct MessageResponse : public ocn::base::facility::Json
{
    std::string Addr;
    std::string Port;
private:
    void assign()
    {
        JSON_ADD_STRING(Addr);
        JSON_ADD_STRING(Port);
    }
};


#endif
