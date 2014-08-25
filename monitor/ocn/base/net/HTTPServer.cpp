#include <ocn/base/net/HTTPServer.h>
#include <ocn/base/facility/Exception.h>
#include <ocn/base/facility/log.h>

using namespace ocn::base::facility;

namespace ocn
{
namespace base
{
namespace net
{
HTTPServer::HTTPServer(int threads)
    : m_threadNumber(threads)
{
    BOOST_ASSERT(m_threadNumber < THREAD_SIZE);

    for (int i = 0; i < m_threadNumber; i++)
    {
        m_HTTPConnectionManager[i].reset(new HTTPConnectionManager());
    }
}

HTTPServer::~HTTPServer()
{
}

int HTTPServer::serv(std::string ip, int port)
{
    int r;
    
    m_listenedSocket.reset(new StreamSocket(ip, port));

    for (int i = 0; i < m_threadNumber; i++) 
    {
        struct event_base *base = event_base_new();
        if (base == NULL)
        {
            return -1;
        }

        struct evhttp *httpd = evhttp_new(base);
        if (httpd == NULL) 
        {
            return -1;
        }
		
        r = evhttp_accept_socket(httpd, m_listenedSocket->getStreamSocket());
        if (r != 0) 
        {
            return -1;
        }
        
        evhttp_set_gencb(httpd, HTTPServer::GenericHandler, (void*)(m_HTTPConnectionManager[i].get()));
        m_thread[i].reset(new boost::thread(boost::bind(HTTPServer::Dispatch, base)));
    }

    /*
    for (int i = 0; i < m_threadNumber; i++) 
    {
        m_thread[i]->join();
    }
    */

    return 0;
}

void* HTTPServer::Dispatch(void *arg) 
{
    LOG_R_I("Create HTTPServer thread id [" << boost::this_thread::get_id() << "].");  
    event_base_dispatch((struct event_base *)arg);
    return NULL;
}

void HTTPServer::GenericHandler(struct evhttp_request *req, void *arg) 
{
    struct evkeyvalq *headers;
    struct evkeyval *header;
    struct evbuffer *buf;
    int     contentLength = 0;
    std::string contentValue;

    headers = evhttp_request_get_input_headers(req);
	for (header = headers->tqh_first; header; header = header->next.tqe_next) 
    {
        if(!strcmp("Content-length",header->key) || !strcmp("Content-Length",header->key))
        {
            contentLength = atoi(header->value);
            //printf("  %s: %s\n", header->key, header->value);
        }
	}

    buf = evhttp_request_get_input_buffer(req);
    while (evbuffer_get_length(buf)) 
    {
        int n;
        char cbuf[128];
        n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
        if (n > 0)
        {
            contentValue.append(cbuf, n);
            //(void) fwrite(cbuf, 1, n, stdout);
        }
	}
    //BOOST_ASSERT(contentLength == contentValue.size());
    if(contentLength != contentValue.size())
    {
        LOG_R_E("ContentLength is [ " << contentLength << "], but actually recv  [" << contentValue.size() << "].");
    }

    ((HTTPConnectionManager*)arg)->newRequest(req, evhttp_request_get_uri(req), contentValue);
}

void HTTPServer::registerHandler(Handler* handler)
{
    for (int i = 0; i < m_threadNumber; i++)
    {
        m_HTTPConnectionManager[i]->registerHandler(handler->clone());
    }
}

}
}
}

