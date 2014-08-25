#include <iostream>
#include "HeartbeatHandler.h"
#include "MessageHandler.h"
#include "EncoderManager.h"

#include <glog/logging.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <ocn/base/facility/log.h>
#include <ocn/base/net/HTTPServer.h>

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#define CONTRL_C_HANDLE() signal(3, _exit)
#endif

#define SERVICE_NAME _T("Monitor Service")
SERVICE_STATUS g_ServiceStatus = {0};
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;
VOID WINAPI ServiceMain (DWORD argc, LPTSTR *argv);
VOID WINAPI ServiceCtrlHandler (DWORD);

#define WIN_SERVICE


#ifdef _MSC_VER
std::string LogCFG_File = "config\\log.property";
//std::string SysConfigure_File = "config\\sys.cfg";
std::wstring LogAppender = L"A3";
#elif defined __GNUC__
std::string LogCFG_File = "config/log.property";
//std::string SysConfigure_File = "config/sys.cfg";
std::string LogAppender = "A3";
#endif
std::string Exe_Path;

using namespace std;

DWORD WINAPI ServiceWorkerThread (LPVOID lpParam)
{
    OutputDebugString(_T("Encoder Engine Service: ServiceWorkerThread: Entry"));
	LOG_R_I("Encoder Engine Service: ServiceWorkerThread: Entry");

    try
    {
        ocn::base::net::HTTPServer httpd(1);

        LOG(INFO) << "Setup EncoderManager...";
        EncoderManager::instance().setExePath(Exe_Path);
        EncoderManager::instance().start();        

        LOG(INFO) << "Start HTTP Server...";
        httpd.registerHandler(new HeartbeatHandler());
        httpd.registerHandler(new MessageHandler());
        
        //evhttp_connection_get_peer

        //cout << "----------" << endl;

        httpd.serv("0.0.0.0", 9090);
        
        
#ifndef WIN_SERVICE
		while(1)
#else
		// Periodically check if the service has been requested to stop
		while (WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0)
#endif
		{
			// 刷新log
			Sleep(5000);
		}
	}
    catch(std::exception& e)
    {
        LOG_R_F("Exception occured : " << e.what());
    }
    catch(...)
    {
        LOG_R_F("Unpredictable exception occured.");        
    }

    OutputDebugString(_T("Encoder Engine Service: ServiceWorkerThread: Exit"));
    LOG_R_I("Encoder Engine Service: ServiceWorkerThread: Exit");

    return ERROR_SUCCESS;
}

int main(int argc, char* argv[])
{
#ifdef _WIN32
	WSADATA WSAData;
	WSAStartup(0x101, &WSAData);
    CONTRL_C_HANDLE();
#endif

	// 初始化glog 
	google::InitGoogleLogging("monitor");

	// 获取本可执行文件目录
	boost::filesystem::path  full_path = boost::filesystem::system_complete(boost::filesystem::path(argv[0]));
    Exe_Path = full_path.parent_path().string();

	// 设置INFO以上log的存放位置
	google::SetLogDestination(google::GLOG_INFO, std::string(Exe_Path + "\\log\\").c_str());

    //LOG_R_I("Build date is [" << __DATE__ << " - " << __TIME__ << "].");
    //LOG_R_I("Build date is " << "qiuzhiguang");

#ifndef WIN_SERVICE
	ServiceWorkerThread(NULL);
#else
    OutputDebugString(_T("Encoder Engine Service : Main: Entry"));
    // 定义服务名字和服务入口
    SERVICE_TABLE_ENTRY ServiceTable[] =
    {
        {SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) ServiceMain},
        {NULL, NULL}
    };
    if (StartServiceCtrlDispatcher (ServiceTable) == FALSE)
    {
       OutputDebugString(_T("Encoder Engine Service: Main: StartServiceCtrlDispatcher returned error"));
       return GetLastError ();
    }
    OutputDebugString(_T("Encoder Engine Service: Main: Exit"));
#endif
    return 0;
}

VOID WINAPI ServiceMain (DWORD argc, LPTSTR *argv)
{
    DWORD Status = E_FAIL;
    OutputDebugString(_T("Encoder Engine Service: ServiceMain: Entry"));
    LOG_R_I("Encoder Engine Service: ServiceMain: Entry");
    // 注册服务
    g_StatusHandle = RegisterServiceCtrlHandler (SERVICE_NAME, ServiceCtrlHandler);
    if (g_StatusHandle == NULL)
    {
        OutputDebugString(_T("Encoder Engine Service: ServiceMain: RegisterServiceCtrlHandler returned error"));
        LOG_R_E("Encoder Engine Service: ServiceMain: RegisterServiceCtrlHandler returned error");
        goto EXIT;
    }
    // Tell the service controller we are starting
    ZeroMemory (&g_ServiceStatus, sizeof (g_ServiceStatus));
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;
    if (SetServiceStatus (g_StatusHandle, &g_ServiceStatus) == FALSE)
    {
        OutputDebugString(_T("Encoder Engine Service: ServiceMain: SetServiceStatus returned error"));
        LOG_R_I("Encoder Engine Service: ServiceMain: SetServiceStatus returned error");
    }
    /*
     * Perform tasks neccesary to start the service here
     */
    OutputDebugString(_T("Encoder Engine Service: ServiceMain: Performing Service Start Operations"));
    LOG_R_I("Encoder Engine Service: ServiceMain: Performing Service Start Operations");
    // Create stop event to wait on later.
    g_ServiceStopEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
    if (g_ServiceStopEvent == NULL)
    {
        OutputDebugString(_T("Encoder Engine Service: ServiceMain: CreateEvent(g_ServiceStopEvent) returned error"));
        LOG_R_I("Encoder Engine Service: ServiceMain: CreateEvent(g_ServiceStopEvent) returned error");
        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = GetLastError();
        g_ServiceStatus.dwCheckPoint = 1;
        if (SetServiceStatus (g_StatusHandle, &g_ServiceStatus) == FALSE)
     {
      OutputDebugString(_T("Encoder Engine Service: ServiceMain: SetServiceStatus returned error"));
            LOG_R_E("Encoder Engine Service: ServiceMain: SetServiceStatus returned error");
     }
        goto EXIT;
    }
    // Tell the service controller we are started
    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;
    if (SetServiceStatus (g_StatusHandle, &g_ServiceStatus) == FALSE)
    {
     OutputDebugString(_T("Encoder Engine Service: ServiceMain: SetServiceStatus returned error"));
        LOG_R_I("Encoder Engine Service: ServiceMain: SetServiceStatus returned error");
    }
    // Start the thread that will perform the main task of the service
    HANDLE hThread = CreateThread (NULL, 0, ServiceWorkerThread, NULL, 0, NULL);
    OutputDebugString(_T("Encoder Engine Service: ServiceMain: Waiting for Worker Thread to complete"));
    LOG_R_I("Encoder Engine Service: ServiceMain: Waiting for Worker Thread to complete");
    // Wait until our worker thread exits effectively signaling that the service needs to stop
    WaitForSingleObject (hThread, INFINITE);
   
    OutputDebugString(_T("Encoder Engine Service: ServiceMain: Worker Thread Stop Event signaled"));
    LOG_R_I("Encoder Engine Service: ServiceMain: Worker Thread Stop Event signaled");
   
   
    /*
     * Perform any cleanup tasks
     */
    OutputDebugString(_T("Encoder Engine Service: ServiceMain: Performing Cleanup Operations"));
    LOG_R_I("Encoder Engine Service: ServiceMain: Performing Cleanup Operations");
    CloseHandle (g_ServiceStopEvent);
    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 3;
    if (SetServiceStatus (g_StatusHandle, &g_ServiceStatus) == FALSE)
    {
     OutputDebugString(_T("Encoder Engine Service: ServiceMain: SetServiceStatus returned error"));
        LOG_R_I("Encoder Engine Service: ServiceMain: SetServiceStatus returned error");
    }
   
    EXIT:
    OutputDebugString(_T("Encoder Engine Service: ServiceMain: Exit"));
    LOG_R_I("Encoder Engine Service: ServiceMain: Exit");
    return;
}

VOID WINAPI ServiceCtrlHandler (DWORD CtrlCode)
{
    OutputDebugString(_T("Encoder Engine Service: ServiceCtrlHandler: Entry"));
    LOG_R_I("Encoder Engine Service: ServiceCtrlHandler: Entry");
    switch (CtrlCode)
    {
        case SERVICE_CONTROL_STOP :
            OutputDebugString(_T("Encoder Engine Service: ServiceCtrlHandler: SERVICE_CONTROL_STOP Request"));
            LOG_R_I("Encoder Engine Service: ServiceCtrlHandler: SERVICE_CONTROL_STOP Request");
            if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
                break;
            /*
             * Perform tasks neccesary to stop the service here
             */
       
            g_ServiceStatus.dwControlsAccepted = 0;
            g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
            g_ServiceStatus.dwWin32ExitCode = 0;
            g_ServiceStatus.dwCheckPoint = 4;
            if (SetServiceStatus (g_StatusHandle, &g_ServiceStatus) == FALSE)
            {
	            OutputDebugString(_T("Encoder Engine Service: ServiceCtrlHandler: SetServiceStatus returned error"));
                LOG_R_I("Encoder Engine Service: ServiceCtrlHandler: SetServiceStatus returned error");
            }
            
            // This will signal the worker thread to start shutting down
            SetEvent (g_ServiceStopEvent);
            break;
        default:
            break;
    }
    OutputDebugString(_T("Encoder Engine Service: ServiceCtrlHandler: Exit"));
    LOG_R_I("Encoder Engine Service: ServiceCtrlHandler: Exit");
}
