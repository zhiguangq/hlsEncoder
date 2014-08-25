#ifndef MESSAGEREQUEST_H
#define MESSAGEREQUEST_H

#include <string>
#include <ocn/base/facility/Json.h>
#include "APIDefine.h"

struct MessageRequest : public ocn::base::facility::Json
{
    std::string             Message;

private:
	void assign()
	{
        JSON_ADD_STRING(Message);
	}
};


#endif
