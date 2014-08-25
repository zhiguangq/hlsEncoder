#ifndef CONFIGURE_H
#define CONFIGURE_H

#include "Singleton.h"
#include <Poco/DOM/NamedNodeMap.h>

class Configure : public Singleton<Configure>
{
    friend class Singleton<Configure>;
	struct NetworkPathInfo
	{
		std::string objectName;
		std::string path;
		std::string username;	//共享资源授权的用户  
		std::string password;	//共享资源授权用户的密码  
	};
	struct OutputStream
	{
		std::string enable;
		std::string bitrate;
		std::string resolutionRatio;
	};
private:
    Configure(void);
	void getNetWorkPathAttr(Poco::XML::NamedNodeMap*, struct NetworkPathInfo*);
	void getOutputStreamAttr(Poco::XML::NamedNodeMap* map, struct OutputStream*);

    void loadChannelList(void);
public:
    ~Configure(void);
    bool load(void);

    std::string     m_encodeChannel;
    std::string     m_encodeChannelID;
    std::string     m_encodeChannelPrefix;
    struct NetworkPathInfo     m_inputTSPath;
    struct NetworkPathInfo     m_inputXMLPath;
    struct NetworkPathInfo     m_outputTSPath;
    struct NetworkPathInfo     m_outputXMLPath;
	struct OutputStream        m_outputStream[3];

    int             m_keepDateTime;
};

#endif

