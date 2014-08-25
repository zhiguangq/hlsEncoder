/**
 * Title: API for error
 * Description: Handler all api interface
 * Copyright: Copyright (c) 2013 ocn (All rights reserved)
 * Company: ocn
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-9-24
 * Version: 1.0
 * History: [1.0 2013-9-24]
 */


#ifndef GENERALERROR_NET_BASE_OCN_H
#define GENERALERROR_NET_BASE_OCN_H

#include <string>

#include <boost/shared_ptr.hpp>

#include <ocn/base/facility/Json.h>

namespace ocn
{
namespace base
{
namespace net
{

struct GeneralError : public ocn::base::facility::Json
{
    GeneralError(){}
    GeneralError(std::string rc, std::string in)
        : Result(rc)
        , Info(in)
    {
    	//resultCode.reset(new std::string(rc));
    	//info.reset(new std::string(in));
    }

    void setResultCode(const std::string& rc)
    {
    	//resultCode.reset(new std::string(rc));
        Result = rc;
    }

    void setInfo(const std::string& in)
    {
    	//info.reset(new std::string(in));
        Info = in;
    }

    //boost::shared_ptr<std::string> resultCode;
    //boost::shared_ptr<std::string> info;
    std::string     Result;
    std::string     Info;

protected:
    void assign()
    {
        //JSON_ADD_PARA(resultCode);
        //JSON_ADD_PARA(info);
        JSON_ADD_STRING(Result);
        JSON_ADD_STRING(Info);
    }
};

} // http
} // base
} // net

#endif
