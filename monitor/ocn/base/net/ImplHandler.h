/**
 * Title: API Handler
 * Description: Handler all api interface
 * Copyright: Copyright (c) 2013 ocn (All rights reserved)
 * Company: ocn
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-9-24
 * Version: 1.0
 * History: [1.0 2013-9-24]
 */

#ifndef IMPLHANDLER_NET_BASE_OCN_H
#define IMPLHANDLER_NET_BASE_OCN_H


#include <string>

#include <ocn/base/net/GeneralError.h>
#include <ocn/base/net/Handler.h>
#include <ocn/base/net/HTTPReply.h>

namespace ocn
{
namespace base
{
namespace net
{

template<typename Request, typename Response>
class ImplHandler : public ocn::base::net::Handler
{
public:
	ImplHandler(){}

    virtual ~ImplHandler(){}

protected:
    Request m_request;
    Response m_response;

    virtual void getPeerAddress(const std::string& peerAddress){}
    virtual bool impl_handle() = 0;
    virtual bool impl_check()
    {
    	return true;
    }

    virtual bool handle(const std::string& peerAddress, std::string& outJson)
    {
    	try
		{
            getPeerAddress(peerAddress);
    		impl_handle();
			outJson = m_response.toJson();
			//LOG_T(_T("the Result of Handle:\n")<<outJson);
			return true;
		}
		catch(...)
		{
			outJson = m_response.toJson();
			//LOG_W(_T("throw unknown exception"));
			return false;
		}
    }

    virtual bool parser(const std::string& inJson, std::string& outJson)
    {
    	try
    	{
    		//re-initialization members, or else the cur-value is pre-value
    		m_request = Request();
    		m_response = Response();

            if(!inJson.empty() && m_request.fromJson(inJson) == false)
            {
                outJson = ocn::base::net::GeneralError("JSON_ERROR", "parse json error").toJson();
                return false;
            }
    		//LOG_T(_T("convert json string to business object successful inJson:[\n")<<inJson<<_T("]"));
    		//LOG_T(_T("the request object:==>")<<m_request.toJson());
    		return true;
    	}
    	catch(...)
    	{
    		//LOG_W(_T("convert json string to business object fail inJson:[")<<inJson<<_T("]"));
    		outJson = ocn::base::net::GeneralError("PARAM_ERROR", "parse json error").toJson();
    		return false;
    	}
    }

    virtual bool check(std::string& outJson)
    {
    	if (!impl_check())
    	{
    		outJson = m_response.toJson();
    		return false;
    	}
    	return true;
    }
};

}
}
}

#endif
