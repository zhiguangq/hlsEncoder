/**
 * Title: HTTP Server
 * Description: Handler all api interface
 * Copyright: Copyright (c) 2013 ocn (All rights reserved)
 * Company: ocn
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-9-24
 * Version: 1.0
 * History: [1.0 2013-9-24]
 */

#ifndef HANDLER_NET_BASE_OCN_H
#define HANDLER_NET_BASE_OCN_H

#include <string>

namespace ocn
{
namespace base
{
namespace net
{

class Handler
{
public:
	Handler(){}

    virtual ~Handler(){}

    bool operator()(const std::string& peerAddress, const std::string& inJson, std::string& outJson)
	{
    	if (!parser(inJson, outJson))
    	{
    		return false;
    	}

    	if (!check(outJson))
    	{
    		return false;
    	}

    	return handle(peerAddress, outJson);
	}

    virtual std::string name() = 0;
    virtual Handler* clone(void) = 0;
protected:
    virtual bool handle(const std::string& peerAddress, std::string& outJson) = 0;
    virtual bool parser(const std::string& inJson, std::string& outJson) = 0;
    virtual bool check(std::string& outJson) = 0;
};

} // http
} // base
} // net

#endif
