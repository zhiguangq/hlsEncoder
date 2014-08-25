/**
 * Title: HTTP Server
 * Description: 定义HTTP回复内容
 * Copyright: Copyright (c) 2013 ocn (All rights reserved)
 * Company: ocn
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-9-24
 * Version: 1.0
 * History: [1.0 2013-9-24]
 */

#ifndef HTTPREPLY_NET_BASE_OCN_H
#define HTTPREPLY_NET_BASE_OCN_H

#include <string>
#include <vector>

#include <boost/asio/buffer.hpp>

#include <ocn/base/net/HTTPHeader.h>

namespace ocn
{
namespace base
{
namespace net
{

class HTTPReply
{
public:
    typedef std::vector<boost::asio::const_buffer> ConstBufferVector;
    /// The status of the reply
    enum status_type
    {
        ok = 200,
        created = 201,
        accepted = 202,
        no_content = 204,
        multiple_choices = 300,
        moved_permanently = 301,
        moved_temporarily = 302,
        not_modified = 304,
        bad_request = 400,
        unauthorized = 401,
        forbidden = 403,
        not_found = 404,
        internal_server_error = 500,
        not_implemented = 501,
        bad_gateway = 502,
        service_unavailable = 503
    }status;

    /// The headers to be include in the reply.
    std::vector<HTTPHeader> m_headers;

    /// The content to be sent in the reply.
    std::string m_content;

    /// Convert the reply into a vector of buffers. The buffer do not own the
    /// underlying memory blocks, therefor the reply object must remain valid and
    /// not be changed until the write operation has completed.
    ConstBufferVector ReplyBuffers();

private:
    boost::asio::const_buffer statusToString(status_type status);
};

} // http    
} // base
} // net

#endif
