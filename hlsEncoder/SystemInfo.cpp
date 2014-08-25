#include "SystemInfo.h"
#include "Configure.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <glog/logging.h>

#ifdef WIN32
#include <Windows.h>
#endif
#include <vector>

SystemInfo::SystemInfo(void)
{
}

SystemInfo::~SystemInfo(void)
{
}

LPWSTR SystemInfo::stringToLPCWSTR(std::string orig)
{
	size_t origsize = orig.length() + 1;
    const size_t newsize = 100;
    size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length()-1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}

void SystemInfo::init(std::string exe_name)
{
	m_executeName = exe_name;
}

bool SystemInfo::initNetWorkDisk(void)
{
	// 把cifs目录网络磁盘映射成本地驱动器
	LOG(INFO) << "init NetWorkDisk." ;

	std::vector<struct Configure::NetworkPathInfo*> pathVector;
	pathVector.push_back(&(Configure::instance().m_inputTSPath));
	pathVector.push_back(&(Configure::instance().m_inputXMLPath));
	pathVector.push_back(&(Configure::instance().m_outputTSPath));
	pathVector.push_back(&(Configure::instance().m_outputXMLPath));
  
	for(unsigned int i = 0; i < pathVector.size(); i++)
	{
		LOG(INFO) << "mount : " << pathVector[i]->path << " as " << pathVector[i]->objectName << " username : " \
			<< pathVector[i]->username << " password : " << pathVector[i]->password;
		NETRESOURCE net_Resource;
		memset(&net_Resource, 0, sizeof (NETRESOURCE));
		net_Resource.dwDisplayType = RESOURCEDISPLAYTYPE_DIRECTORY;  
		net_Resource.dwScope = RESOURCE_CONNECTED;  
		net_Resource.dwType = RESOURCETYPE_ANY;  
		net_Resource.dwUsage = 0;
		net_Resource.lpComment = TEXT("");  
		net_Resource.lpLocalName = this->stringToLPCWSTR(pathVector[i]->objectName);  //映射成本地驱动器
		net_Resource.lpProvider= NULL;  
		net_Resource.lpRemoteName = this->stringToLPCWSTR(pathVector[i]->path);
		DWORD dwFlags = CONNECT_UPDATE_PROFILE;
        DWORD dw;

        // 先把旧的映射退掉
        dw = WNetCancelConnection2(net_Resource.lpLocalName,  dwFlags, FALSE); 
        LOG(INFO) << "Connection closed for drive [" << pathVector[i]->objectName.c_str() << "].  return code : " << dw;

		dw = WNetAddConnection2(&net_Resource, stringToLPCWSTR(pathVector[i]->password), stringToLPCWSTR(pathVector[i]->username), dwFlags); 

		if(dw != ERROR_SUCCESS && dw != ERROR_ALREADY_ASSIGNED)
		{
			LOG(ERROR) << "mount [" << pathVector[i]->path.c_str() << "] failed. Error : " << dw;
			return false;
		}

        LOG(INFO) << "Connection to drive [" << pathVector[i]->objectName.c_str() \
            << "]   [" << pathVector[i]->path.c_str() << "] sucessful.";
	}
	return true;
}

std::string SystemInfo::getExecutePath(void)
{
	// 获取本可执行文件目录
	boost::filesystem::path  full_path = boost::filesystem::system_complete(boost::filesystem::path(m_executeName));
	return full_path.parent_path().string();
}

