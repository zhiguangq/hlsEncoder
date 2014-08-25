#include <iostream>
#include "HeartbeatHandler.h"
#include "EncoderManager.h"

#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/dailyrollingfileappender.h>

#include <kugou/base/facility/log.h>
#include <kugou/base/net/HTTPServer.h>

#ifdef _WIN32
#include <windows.h>
#define CONTRL_C_HANDLE() signal(3, _exit)
#endif


#ifdef _MSC_VER
std::string LogCFG_File = "config\\log.property";
//std::string SysConfigure_File = "config\\sys.cfg";
std::wstring LogAppender = L"A3";
#elif defined __GNUC__
std::string LogCFG_File = "config/log.property";
//std::string SysConfigure_File = "config/sys.cfg";
std::string LogAppender = "A3";
#endif

using namespace std;

int main(int argc, char* argv[])
{
#ifdef _WIN32
	WSADATA WSAData;
	WSAStartup(0x101, &WSAData);
    CONTRL_C_HANDLE();
#endif

	log4cxx::PropertyConfigurator::configure(LogCFG_File);
    log4cxx::DailyRollingFileAppenderPtr t_appender(log4cxx::Logger::getRootLogger()->getAppender(LogAppender));
        
    LOG_R_I("Build date is [" << __DATE__ << " - " << __TIME__ << "].");

    try
    {
        kugou::base::net::HTTPServer httpd(1);

        EncoderManager::instance().start();

        httpd.registerHandler(new HeartbeatHandler());
        
        //evhttp_connection_get_peer

        //cout << "----------" << endl;

        httpd.serv("0.0.0.0", 9090);
    }
    catch(std::exception& e)
    {
        LOG_R_F("Exception occured : " << e.what());
    }
    catch(...)
    {
        LOG_R_F("Unpredictable exception occured.");        
    }

    return 0;
}
