/**
 * Title: HTTP Request handler
 * Description: ¥¶¿ÌHTTP«Î«Û.
 * Copyright: Copyright (c) 2013 ocn (All rights reserved)
 * Company: ocn
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-9-24
 * Version: 1.0
 * History: [1.0 2013-9-24]
 */

#ifndef HTTPREQUESTHANDLER_NET_BASE_OCN_H
#define HTTPREQUESTHANDLER_NET_BASE_OCN_H

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <ocn/base/net/HTTPReply.h>
#include <ocn/base/net/Handler.h>

namespace ocn
{
namespace base
{
namespace net
{

class HTTPReply;
class HTTPRequest;
class Handler;

class HTTPRequestHandler
    : private boost::noncopyable
{
public:
	/**
	 * Construct with a directory containing files to be served.
	 *
	 * @param doc_root document diretory.
	 */
    HTTPRequestHandler(const std::string& doc_root);

	/**
	 * Handle a request and produce a reply.
	 *
	 * @param req input requent.
	 * @param rep output reply.
	 */
    //void handleRequest(const HTTPRequest& req, HTTPReply& rep);
    void handleRequest(std::string peerAddress, std::string uri, std::string content, HTTPReply& rep);

    /**
	 * Register request handler.
	 *
	 *
	 */
    void registerHandler(Handler* handler);

    //Dispatch the request
    HTTPReply::status_type dispatch(std::string peerAddress, const std::string& uri, const std::string& contentJson, std::string& outJson);

private:
    /// The directory containing the files to be served.
    std::string m_docRoot;

    typedef std::map<std::string, boost::shared_ptr<Handler> > HandlerMap;// HandlerMap

    HandlerMap m_handlers;

    /**
	 * Perform URL-decoding on a string.
	 *
	 * @param req input url.
	 * @param rep output url.
	 * @return false if the encoding was invalid.
	 */
    static bool urlDecode(const std::string& in, std::string& out);
};
    
} // http    
} // base
} // net


#endif
