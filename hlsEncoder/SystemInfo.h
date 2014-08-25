#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <iostream>
#include "Singleton.h"

#ifdef WIN32
#include <Windows.h>
#endif

class SystemInfo : public Singleton<SystemInfo>
{
	friend class Singleton<SystemInfo>;
public:
	~SystemInfo(void);

	void init(std::string exe_name);
	bool initNetWorkDisk(void);
	std::string getExecutePath(void);
	LPWSTR stringToLPCWSTR(std::string orig);
private:
	SystemInfo(void);
	std::string m_executeName;
};

#endif

