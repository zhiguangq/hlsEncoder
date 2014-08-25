#ifdef WIN32
#include <Windows.h>
#include <tchar.h>
#endif

#include <glog/logging.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <glog/logging.h>

#include "EncoderWorker.h"
#include "Configure.h"
#include "FileDetection.h"
#include "SystemInfo.h"
#include "DeleteFileDetection.h"
#include "HeartbeatWorker.h"


#define SERVICE_NAME _T("Encoder Engine Service")
SERVICE_STATUS g_ServiceStatus = {0};
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;
VOID WINAPI ServiceMain (DWORD argc, LPTSTR *argv);
VOID WINAPI ServiceCtrlHandler (DWORD);

using namespace std;
#define WIN_SERVICE

DWORD WINAPI ServiceWorkerThread (LPVOID lpParam)
{
    OutputDebugString(_T("Encoder Engine Service: ServiceWorkerThread: Entry"));
    LOG(INFO) << (("Encoder Engine Service: ServiceWorkerThread: Entry"));

	try
	{
		LOG(INFO) << ("Encoder Engine Service Start.") << endl;

		// 导入配置文件
		if(!Configure::instance().load())
		{
			LOG(ERROR) << "load configure file error.";
			return ERROR_SUCCESS;
		}
        
		// 初始化网络磁盘
		if(!SystemInfo::instance().initNetWorkDisk())
		{
			LOG(ERROR) << "init NetWorkDisk error.";
			return ERROR_SUCCESS;
		}

		// 启动文件扫描模块
		LOG(INFO) << ("Start detection.");
		FileDetection fd(Configure::instance().m_inputXMLPath.objectName + "\\");
		fd.start();

        // 启动文件删除线程
        if(Configure::instance().m_keepDateTime > -1)
        {
            LOG(INFO) << "Start delete file thread.";
		    DeleteFileDetection dfd;
		    dfd.start();
        }
        else
        {
            LOG(INFO) << "KeepingDateTime is set to [" << Configure::instance().m_keepDateTime << "]. delete file thread stop.";
        }
        
        // 启动心跳线程
        HeartbeatWorker hw;
        hw.start();
        
		// 启动编码模块
		EncoderWorker ew;
		ew.start();
        
#ifndef WIN_SERVICE
		while(1)
#else
		// Periodically check if the service has been requested to stop
		while (WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0)
#endif
		{
			// 刷新log
			google::FlushLogFiles(google::GLOG_INFO);
			Sleep(5000);
		}
	}
	catch(std::exception& e)
	{
		LOG(ERROR) << "Exception happan : " << e.what();
	}
	catch(...)
	{
		LOG(ERROR) << "Unknow exception happan.";
	}

    OutputDebugString(_T("Encoder Engine Service: ServiceWorkerThread: Exit"));
    LOG(INFO) << (("Encoder Engine Service: ServiceWorkerThread: Exit"));
	google::FlushLogFiles(google::GLOG_INFO);
    return ERROR_SUCCESS;
}

int main(int argc, char* argv[])
{
	// 初始化glog 
	google::InitGoogleLogging("encoder");

	// 初始化系统基础信息
	SystemInfo::instance().init(argv[0]);

	// 设置INFO以上log的存放位置
	google::SetLogDestination(google::GLOG_INFO, std::string(SystemInfo::instance().getExecutePath() + "\\log\\").c_str());

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
    LOG(INFO) << ("Encoder Engine Service: ServiceMain: Entry");
    // 注册服务
    g_StatusHandle = RegisterServiceCtrlHandler (SERVICE_NAME, ServiceCtrlHandler);
    if (g_StatusHandle == NULL)
    {
        OutputDebugString(_T("Encoder Engine Service: ServiceMain: RegisterServiceCtrlHandler returned error"));
        LOG(ERROR) << _T("Encoder Engine Service: ServiceMain: RegisterServiceCtrlHandler returned error");
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
        LOG(ERROR) << _T("Encoder Engine Service: ServiceMain: SetServiceStatus returned error");
    }
    /*
     * Perform tasks neccesary to start the service here
     */
    OutputDebugString(_T("Encoder Engine Service: ServiceMain: Performing Service Start Operations"));
    LOG(INFO) << ("Encoder Engine Service: ServiceMain: Performing Service Start Operations");
    // Create stop event to wait on later.
    g_ServiceStopEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
    if (g_ServiceStopEvent == NULL)
    {
        OutputDebugString(_T("Encoder Engine Service: ServiceMain: CreateEvent(g_ServiceStopEvent) returned error"));
        LOG(ERROR) << _T("Encoder Engine Service: ServiceMain: CreateEvent(g_ServiceStopEvent) returned error");
        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = GetLastError();
        g_ServiceStatus.dwCheckPoint = 1;
        if (SetServiceStatus (g_StatusHandle, &g_ServiceStatus) == FALSE)
     {
      OutputDebugString(_T("Encoder Engine Service: ServiceMain: SetServiceStatus returned error"));
            LOG(ERROR) << _T("Encoder Engine Service: ServiceMain: SetServiceStatus returned error");
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
        LOG(INFO) << (("Encoder Engine Service: ServiceMain: SetServiceStatus returned error"));
    }
    // Start the thread that will perform the main task of the service
    HANDLE hThread = CreateThread (NULL, 0, ServiceWorkerThread, NULL, 0, NULL);
    OutputDebugString(_T("Encoder Engine Service: ServiceMain: Waiting for Worker Thread to complete"));
    LOG(INFO) << (("Encoder Engine Service: ServiceMain: Waiting for Worker Thread to complete"));
    // Wait until our worker thread exits effectively signaling that the service needs to stop
    WaitForSingleObject (hThread, INFINITE);
   
    OutputDebugString(_T("Encoder Engine Service: ServiceMain: Worker Thread Stop Event signaled"));
    LOG(INFO) << (("Encoder Engine Service: ServiceMain: Worker Thread Stop Event signaled"));
   
   
    /*
     * Perform any cleanup tasks
     */
    OutputDebugString(_T("Encoder Engine Service: ServiceMain: Performing Cleanup Operations"));
    LOG(INFO) << (("Encoder Engine Service: ServiceMain: Performing Cleanup Operations"));
    CloseHandle (g_ServiceStopEvent);
    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 3;
    if (SetServiceStatus (g_StatusHandle, &g_ServiceStatus) == FALSE)
    {
     OutputDebugString(_T("Encoder Engine Service: ServiceMain: SetServiceStatus returned error"));
        LOG(INFO) << (("Encoder Engine Service: ServiceMain: SetServiceStatus returned error"));
    }
   
    EXIT:
    OutputDebugString(_T("Encoder Engine Service: ServiceMain: Exit"));
    LOG(INFO) << (("Encoder Engine Service: ServiceMain: Exit"));
    return;
}

VOID WINAPI ServiceCtrlHandler (DWORD CtrlCode)
{
    OutputDebugString(_T("Encoder Engine Service: ServiceCtrlHandler: Entry"));
    LOG(INFO) << (("Encoder Engine Service: ServiceCtrlHandler: Entry"));
    switch (CtrlCode)
 {
     case SERVICE_CONTROL_STOP :
        OutputDebugString(_T("Encoder Engine Service: ServiceCtrlHandler: SERVICE_CONTROL_STOP Request"));
        LOG(INFO) << (("Encoder Engine Service: ServiceCtrlHandler: SERVICE_CONTROL_STOP Request"));
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
            LOG(INFO) << (("Encoder Engine Service: ServiceCtrlHandler: SetServiceStatus returned error"));
  }
        // This will signal the worker thread to start shutting down
        SetEvent (g_ServiceStopEvent);
        break;
     default:
         break;
    }
    OutputDebugString(_T("Encoder Engine Service: ServiceCtrlHandler: Exit"));
    LOG(INFO) << (("Encoder Engine Service: ServiceCtrlHandler: Exit"));
}
