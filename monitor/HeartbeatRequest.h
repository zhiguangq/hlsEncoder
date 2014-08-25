#ifndef HEARTBEATREQUEST_H
#define HEARTBEATREQUEST_H

#include <string>
#include <kugou/base/facility/Json.h>
#include "APIDefine.h"

struct HeartbeatRequest : public kugou::base::facility::Json
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
