#include "HeartbeatWorker.h"
#include <iostream>
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include <glog/logging.h>


HeartbeatWorker::HeartbeatWorker(void)
{
}


HeartbeatWorker::~HeartbeatWorker(void)
{
}

void HeartbeatWorker::run(void)
{
    while(1)
    {
        try
        {
            std::string host = "10.27.69.84";
            int         port = 9090;
            LOG(INFO) << "Send hartbeat to " << host << ":" << port;
            Poco::Net::HTTPClientSession s(host.c_str(), port);
	        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, "/api/monitor/heartbeat.json");
	        std::string body("{\"ID\":1}");
            
	        request.setContentLength((int) body.length());
	        s.sendRequest(request) << body;
	        Poco::Net::HTTPResponse response;
	        std::istream& rs = s.receiveResponse(response);
	        std::ostringstream ostr;
	        Poco::StreamCopier::copyStream(rs, ostr);
        }
        catch(...)
        {
            LOG(INFO) << "Send hartbeat exception.";
        }

        // 每隔1分钟发一次心跳
        Sleep(60 * 1000);
    }
}

void HeartbeatWorker::start(void)
{
    m_thread.reset(new boost::thread( boost::bind(&HeartbeatWorker::run,this)));
}

