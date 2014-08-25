#include <fstream>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>

#include <ocn/base/facility/Base64.h>
#include <ocn/base/net/HTTPReply.h>
#include <ocn/base/net/GeneralError.h>
#include <ocn/base/net/HTTPRequestHandler.h>
#include <ocn/base/facility/log.h>

using namespace ocn::base::facility;

namespace ocn
{
namespace base
{
namespace net
{
    
HTTPRequestHandler::HTTPRequestHandler(const std::string& doc_root)
 : m_docRoot(doc_root)
{    
}

void HTTPRequestHandler::registerHandler(Handler* handler)
{
    LOG_I("register handler ==> [" << handler->name() << "]");
	m_handlers[handler->name()].reset(handler);
}

HTTPReply::status_type HTTPRequestHandler::dispatch(std::string peerAddress, const std::string& uri, const std::string& contentJson, std::string& outJson)
{
    std::string apiName = uri;
    std::string callback;
    std::string inJson = contentJson;
    HTTPReply::status_type ret = HTTPReply::not_found;

    // 兼容GET带参数方式接口，提取出
    boost::regex re("(.*)\\?callback=(.*)&jsonStr=(.*)&(.*)");
    boost::cmatch matches;
    if(boost::regex_search(uri.c_str(), matches, re))
    {
        if(matches.size() == 5)
        {
            apiName = matches[1];
            callback = matches[2];
            std::string base64_inJson = matches[3];

            if(!callback.compare("ipsearch"))
            {
                inJson = base64_inJson;
                boost::replace_all(inJson, "%22", "\"");
            }
            else
            {            
                //反base64编码
                Base64::Base64Decode(base64_inJson, &inJson);
            }
        }
    }

    LOG_T("API[" << apiName << "] ==> " << inJson);

	HandlerMap::iterator iter = m_handlers.find(apiName);
	if (iter != m_handlers.end())
	{
		//找到对应的处理器，调用处理器接口
		(*iter->second)(peerAddress, inJson, outJson);
		ret = HTTPReply::ok;
	}
	else
	{
		//没有找到对应的接口，返回404错误
        LOG_W("API[" << apiName << "] handler not found!");
        outJson = GeneralError("UNKNOWN_REQ", "unknow request").toJson();
		ret = HTTPReply::not_found;
	}

    // 如果是用GET参数方式接口，回复JSON串前加入callback函数
    if(!callback.empty())
    {
        outJson.insert(0, callback.append("(").c_str());
        outJson.append(")");
    }

    return ret;
}

//void HTTPRequestHandler::handleRequest(const HTTPRequest& req, HTTPReply& rep)
void HTTPRequestHandler::handleRequest(std::string peerAddress, std::string uri, std::string content, HTTPReply& rep)
{
    rep.m_headers.resize(1);
    rep.m_headers[0].m_name = "Content-Type";
    rep.m_headers[0].m_value = "application/json; charset=UTF-8";

    // Determine the file extension.
    std::size_t last_slash_pos = uri.find_last_of("/");
    std::size_t last_dot_pos = uri.find_last_of(".");
    std::string extension;
    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
    {
        extension = uri.substr(last_dot_pos + 1);
    }

    std::size_t api_pos = uri.find("/api/", 0);
    if(api_pos != std::string::npos)
    {
        // url是以/api/开头的情况往下
		try
		{
			std::string apiName = uri;
			std::string inJson = content; 
			std::string outJson;

			rep.status = dispatch(peerAddress, apiName, inJson, outJson);;
			rep.m_content.append(outJson);
		}
		catch(...)
		{
			//TODO
            LOG_W("throw unknown exception when handleRequest");
			rep.status = HTTPReply::internal_server_error;
			rep.m_content.append(GeneralError("PARAM_ERROR", "handle request error").toJson());
		}
    }
    else
    {
        // 如果是crossdomain.xml，返回内容
        std::string crossdomainUrl = "/crossdomain.xml";
        if(uri.compare(crossdomainUrl) == 0)
        {
            rep.m_content = "<cross-domain-policy>\r\n<allow-access-from domain=\"*\"/>\r\n</cross-domain-policy>";
            rep.m_headers[0].m_value = "text/xml";
            return;
        }
        else
        {
    	    //404
    	    LOG_W("Request URL is wrong! URL:[" << uri << "]");
    	    rep.status = HTTPReply::not_found;
    	    rep.m_content.append(GeneralError("UNKNOWN_REQ", "unknow request").toJson());
            rep.m_headers[0].m_value = "text/plain";
        }
    }
}

bool HTTPRequestHandler::urlDecode(const std::string& in, std::string& out)
{
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i)
  {
    if (in[i] == '%')
    {
      if (i + 3 <= in.size())
      {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value)
        {
          out += static_cast<char>(value);
          i += 2;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else if (in[i] == '+')
    {
      out += ' ';
    }
    else
    {
      out += in[i];
    }
  }
  return true;
}

} // http    
} // server
} // ocn
