/**
 * Title: HTTP Server
 * Description: 定义HTTP头信息
 * Copyright: Copyright (c) 2013 ocn (All rights reserved)
 * Company: ocn
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-9-24
 * Version: 1.0
 * History: [1.0 2013-9-24]
 */
 
#ifndef HTTPHEADER_NET_BASE_OCN_H
#define HTTPHEADER_NET_BASE_OCN_H

#include <iostream>

namespace ocn
{
namespace base
{
namespace net
{

class HTTPHeader
{
public:
    std::string m_name;
    std::string m_value;
};
    
} // http    
} // base
} // net

#endif
