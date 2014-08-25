#include "XMLInfo.h"
#include "Configure.h"

#include <fstream>
#include <glog/logging.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/NodeIterator.h>
#include <Poco/DOM/NodeFilter.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/SAX/InputSource.h>


XMLInfo::XMLInfo(void)
{
}

XMLInfo::~XMLInfo(void)
{
}


bool XMLInfo::load(std::string inputName)
{
    std::ifstream in(inputName);
    Poco::XML::InputSource src(in);
    Poco::XML::DOMParser parser;
    try
    {
        Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parse(&src);
        Poco::XML::NodeIterator it(pDoc, Poco::XML::NodeFilter::SHOW_ELEMENT);
        Poco::XML::Node* pNode = it.nextNode();

        while (pNode)
        {
            if(pNode->attributes())
            {
                Poco::XML::NamedNodeMap* attrMap = pNode->attributes();
                if(attrMap->getNamedItem("AssetName")->nodeValue().size() > 0)
                {
                    AssetName = attrMap->getNamedItem("AssetName")->nodeValue();
                }

               if(attrMap->getNamedItem("EventName")->nodeValue().size() > 0)
                {
                    EventName = attrMap->getNamedItem("EventName")->nodeValue();
                }

                if(attrMap->getNamedItem("ChannelName")->nodeValue().size() > 0)
                {
                    ChannelName = attrMap->getNamedItem("ChannelName")->nodeValue();
                }

                if(attrMap->getNamedItem("StartTime")->nodeValue().size() > 0)
                {
                    StartTime = attrMap->getNamedItem("StartTime")->nodeValue();
                }

                if(attrMap->getNamedItem("EndTime")->nodeValue().size() > 0)
                {
                    EndTime = attrMap->getNamedItem("EndTime")->nodeValue();
                }

                
                if(ChannelName.compare(Configure::instance().m_encodeChannel) == 0)
                {
					LOG(INFO) << "xml file [" << inputName << "], its channel [" <<  ChannelName << "] OK.";
                    return true;
                }
                else
                {
					//LOG(INFO) << "xml file [" << inputName << "], its channel [" <<  ChannelName << "] is not we want, omit it.";
                    return false;
                }
            }

            pNode = it.nextNode();
        }
    }
    catch(std::exception& e)
    {
        LOG(ERROR) << "parse xml file error " << e.what();
        return false;
    }

    return true;



}
