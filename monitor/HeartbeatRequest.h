#ifndef HEARTBEATREQUEST_H
#define HEARTBEATREQUEST_H

#include <string>
#include <ocn/base/facility/Json.h>
#include "APIDefine.h"

struct HeartbeatRequest : public ocn::base::facility::Json
{
    /*
    int             ID;

    HeartbeatRequest(void)
        : ID(-1)
    {
    }
    */

private:
	void assign()
	{
        //JSON_ADD_INT(ID);
	}
};


#endif
