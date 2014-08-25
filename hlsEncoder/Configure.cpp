#include "Configure.h"
#include "SystemInfo.h"

#include <fstream>
#include <glog/logging.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/NodeIterator.h>
#include <Poco/DOM/NodeFilter.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/SAX/InputSource.h>
#include <boost/lexical_cast.hpp>

Configure::Configure(void)
    : m_keepDateTime(-2)
{
}


Configure::~Configure(void)
{
}

void Configure::getNetWorkPathAttr(Poco::XML::NamedNodeMap* map, struct NetworkPathInfo* info)
{
    if(map)//看看是否真有属性  
    {  
        for(int i = 0 ; i < map->length() ; i++)//属性肯定至少0个，用循环一个个取出  
        {  
            Poco::XML::Node* attr = map->item(i);  
			if(attr->nodeName() == "UserName")
			{
				info->username = attr->nodeValue();
			}
			else if(attr->nodeName() == "Password")
			{
				info->password = attr->nodeValue();
			}
        }//属性结束
    }
}

void Configure::getOutputStreamAttr(Poco::XML::NamedNodeMap* map, struct OutputStream* outputstream)
{
    if(map)//看看是否真有属性  
    {  
        for(int i = 0 ; i < map->length() ; i++)//属性肯定至少0个，用循环一个个取出  
        {  
            Poco::XML::Node* attr = map->item(i);  
			if(attr->nodeName() == "Enable")
			{
				outputstream->enable = attr->nodeValue();
			}
			else if(attr->nodeName() == "BitRate")
			{
				outputstream->bitrate = attr->nodeValue();
			}
			else if(attr->nodeName() == "ResolutionRatio")
			{
				outputstream->resolutionRatio = attr->nodeValue();
			}
        }//属性结束
    }
}

bool Configure::load(void)
{
    std::ifstream in(SystemInfo::instance().getExecutePath() + "\\config\\sys.xml");
    Poco::XML::InputSource src(in);
    Poco::XML::DOMParser parser;

    try
    {
        Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parse(&src);
        Poco::XML::NodeIterator it(pDoc, Poco::XML::NodeFilter::SHOW_ALL);
        Poco::XML::Node* pNode = it.nextNode();


        while (pNode)  
        {  
            if(pNode->nodeName() == "EncodChannel")
            {
                pNode = it.nextNode();
                if(pNode->nodeName() != "#text")
                {
                    continue; //No text node present
                }
                m_encodeChannel = pNode->nodeValue();
				Poco::XML::XMLString tt = Poco::XML::toXMLString(m_encodeChannel);
            }

            if(pNode->nodeName() == "InputTSPath")
            {
				getNetWorkPathAttr(pNode->attributes(),&m_inputTSPath);
                pNode = it.nextNode();
                if(pNode->nodeName() != "#text")
                {
                    continue; //No text node present
                }
				m_inputTSPath.path = pNode->nodeValue();
				m_inputTSPath.objectName = "o:";
            }

            if(pNode->nodeName() == "InputXMLPath")
            {
				getNetWorkPathAttr(pNode->attributes(),&m_inputXMLPath);
                pNode = it.nextNode();
                if(pNode->nodeName() != "#text")
                {
                    continue; //No text node present
                }
				m_inputXMLPath.path = pNode->nodeValue();
				m_inputXMLPath.objectName = "p:";
            }

            if(pNode->nodeName() == "OutputTSPath")
            {
				getNetWorkPathAttr(pNode->attributes(),&m_outputTSPath);
                pNode = it.nextNode();
                if(pNode->nodeName() != "#text")
                {
                    continue; //No text node present
                }
				m_outputTSPath.path = pNode->nodeValue();
				m_outputTSPath.objectName = "q:";
            }

            if(pNode->nodeName() == "OutputXMLPath")
            {
				getNetWorkPathAttr(pNode->attributes(),&m_outputXMLPath);
                pNode = it.nextNode();
                if(pNode->nodeName() != "#text")
                {
                    continue; //No text node present
                }
				m_outputXMLPath.path = pNode->nodeValue();
				m_outputXMLPath.objectName = "r:";
            }

            if(pNode->nodeName() == "OutputStream1")
            {
				getOutputStreamAttr(pNode->attributes(),&m_outputStream[0]);
                pNode = it.nextNode();
                if(pNode->nodeName() != "#text")
                {
                    continue; //No text node present
                }
            }
            if(pNode->nodeName() == "OutputStream2")
            {
				getOutputStreamAttr(pNode->attributes(),&m_outputStream[1]);
                pNode = it.nextNode();
                if(pNode->nodeName() != "#text")
                {
                    continue; //No text node present
                }
            }
            if(pNode->nodeName() == "OutputStream3")
            {
				getOutputStreamAttr(pNode->attributes(),&m_outputStream[2]);
                pNode = it.nextNode();
                if(pNode->nodeName() != "#text")
                {
                    continue; //No text node present
                }
            }
            if(pNode->nodeName() == "Storage")
            {
                if(pNode->attributes())//看看是否真有属性  
                {  
                    for(int i = 0 ; i < pNode->attributes()->length() ; i++)
                    {  
                        Poco::XML::Node* attr = pNode->attributes()->item(i);  
			            if(attr->nodeName() == "KeepingDateTime")
			            {
				            m_keepDateTime = boost::lexical_cast<int>(attr->nodeValue());
			            }
                    }//属性结束
                }
                pNode = it.nextNode();
                if(pNode->nodeName() != "#text")
                {
                    continue; //No text node present
                }
            }
			

            //LOG(INFO) << "name : " << pNode->nodeName() ;//取出当前节点的Name  
            //LOG(INFO)<< "value : " << pNode->nodeValue() ;//取出当前节点的Value  
            pNode = it.nextNode();//指向下一个node  
        }
    }
    catch(std::exception& e)
    {
        LOG(ERROR) << "parse xml file error." << e.what() ;
        return false;
    }

    this->loadChannelList();

    if(m_encodeChannel.empty() || m_encodeChannelID.empty() || m_encodeChannelPrefix.empty() ||
		m_inputTSPath.path.empty() || m_inputTSPath.objectName.empty() ||
		m_inputXMLPath.path.empty() || m_inputXMLPath.objectName.empty() ||
		m_outputTSPath.path.empty() || m_outputTSPath.objectName.empty() ||
		m_outputXMLPath.path.empty() || m_outputXMLPath.objectName.empty() ||
		m_outputStream[0].bitrate.empty() || m_outputStream[0].enable.empty() || m_outputStream[0].resolutionRatio.empty() ||
		m_outputStream[1].bitrate.empty() || m_outputStream[1].enable.empty() || m_outputStream[1].resolutionRatio.empty() ||
		m_outputStream[2].bitrate.empty() || m_outputStream[2].enable.empty() || m_outputStream[2].resolutionRatio.empty() ||
        m_keepDateTime == -2)
    {
        return false;
    }

    LOG(INFO) << "EncodChannel [" << Configure::instance().m_encodeChannel << "] ID[" << m_encodeChannelID << "] Prefix[" << m_encodeChannelPrefix << "].";
	LOG(INFO) << "InputTSPath [" << Configure::instance().m_inputTSPath.path << "]" << " as [" << Configure::instance().m_inputTSPath.objectName << "]";
	LOG(INFO) << "InputXMLPath [" << Configure::instance().m_inputXMLPath.path << "]" << " as [" << Configure::instance().m_inputXMLPath.objectName << "]";
	LOG(INFO) << "OutputTSPath [" << Configure::instance().m_outputTSPath.path << "]" << " as [" << Configure::instance().m_outputTSPath.objectName << "]";
	LOG(INFO) << "OutputXMLPath [" << Configure::instance().m_outputXMLPath.path << "]" << " as [" << Configure::instance().m_outputXMLPath.objectName << "]";
	LOG(INFO) << "OutputStream1 [" << Configure::instance().m_outputStream[0].enable << ", " << Configure::instance().m_outputStream[0].bitrate << ", " << Configure::instance().m_outputStream[0].resolutionRatio << "]";
    LOG(INFO) << "OutputStream2 [" << Configure::instance().m_outputStream[1].enable << ", " << Configure::instance().m_outputStream[1].bitrate << ", " << Configure::instance().m_outputStream[1].resolutionRatio << "]";
	LOG(INFO) << "OutputStream3 [" << Configure::instance().m_outputStream[2].enable << ", " << Configure::instance().m_outputStream[2].bitrate << ", " << Configure::instance().m_outputStream[2].resolutionRatio << "]";
	LOG(INFO) << "stoage keep DateTime [" << m_keepDateTime << "].";

    return true;
}

void Configure::loadChannelList(void)
{
    std::ifstream in(SystemInfo::instance().getExecutePath() + "\\config\\channelList.xml");
    Poco::XML::InputSource src(in);
    Poco::XML::DOMParser parser;

    try
    {
        Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parse(&src);
        Poco::XML::NodeIterator it(pDoc, Poco::XML::NodeFilter::SHOW_ALL);
        Poco::XML::Node* pNode = it.nextNode();

        static int index = 0;
		std::string channelInfo[3];
        while (pNode)  
        {
            if(pNode->nodeName() == "Row")
            {
                pNode = it.nextNode();
                if(pNode->nodeName() != "#text")
                {
                    continue; //No text node present
                }
                index = 0;
                if(m_encodeChannel == channelInfo[1])
                {
                    // 找到频道对照表
                    m_encodeChannelID = channelInfo[0];
                    m_encodeChannelPrefix = channelInfo[2];
                    LOG(INFO) << "get channel info : ID [" << m_encodeChannelID << "] Prefix[" << m_encodeChannelPrefix << "].";
                    return;
                }
            }
            if(pNode->nodeName() == "Cell")
            {
                pNode = it.nextNode();
                if(pNode->nodeName() != "#text")
                {
                    pNode = it.nextNode();
                    continue; //No text node present
                }
                channelInfo[index++] = pNode->nodeValue();
            }

            pNode = it.nextNode();//指向下一个node
        }
    }
    catch(std::exception& e)
    {
        LOG(ERROR) << "parse channelList xml file error." << e.what() ;
    }
}

