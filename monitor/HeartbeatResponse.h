#ifndef HEARTBEATRESPONSE_H
#define HEARTBEATRESPONSE_H

#include <string>
#include <vector>
#include <kugou/base/facility/Json.h>

struct HeartbeatResponse : public kugou::base::facility::Json
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
