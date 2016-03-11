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
#include <Poco/DateTime.h>
#include <Poco/RegularExpression.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/Foundation.h>


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
            /*
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
            */
            if (!(pNode->nodeName().compare("Content")) && pNode->attributes()){
                Poco::XML::NamedNodeMap* attrMap = pNode->attributes();
                if (AssetName.empty() && \
                    attrMap->getNamedItem("Value") && attrMap->getNamedItem("Value")->nodeValue().size() > 0)
                {
                    AssetName = attrMap->getNamedItem("Value")->nodeValue();

                    if (AssetName.size() == 23){
                        Poco::RegularExpression re("([A-Z0-9]{3})([A-Z0-9]{1})([0-9]{2})([0-9]{3})([0-9]{6})([0-9]{5})");
                        Poco::RegularExpression::MatchVec matches;
                        std::vector<std::string> strings;
                        re.split(AssetName, 0, strings);

                        int year = atoi(strings[3].c_str());
                        int dates = atoi(strings[4].c_str());
                        int hour = atoi(strings[5].substr(0, 2).c_str());
                        int minute = atoi(strings[5].substr(2, 2).c_str());
                        int second = atoi(strings[5].substr(4, 2).c_str());
                        int dur = atoi(strings[6].c_str());
                        Poco::DateTime base1Dt(2000 + year, 1, 1);
                        Poco::DateTime base2Dt(base1Dt.julianDay() + (dates - 1));
                        Poco::DateTime dt(base2Dt.year(), base2Dt.month(), base2Dt.day(), hour, minute, second);

                        ChannelName = strings[1];
                        StartTime = Poco::DateTimeFormatter::format(dt, "%Y-%m-%d %H:%M:%S");
                        dt += Poco::Timespan(dur, 0);
                        EndTime = Poco::DateTimeFormatter::format(dt, "%Y-%m-%d %H:%M:%S");
                    }
                }
            }

            if (!(pNode->nodeName().compare("AMS")) && pNode->attributes()){
                Poco::XML::NamedNodeMap* attrMap = pNode->attributes();
                if (EventName.empty() && \
                    attrMap->getNamedItem("Description") && attrMap->getNamedItem("Description")->nodeValue().size() > 0)
                {
                    EventName = attrMap->getNamedItem("Description")->nodeValue();
                }
            }

            pNode = it.nextNode();
        }

		if(ChannelName.compare(Configure::instance().m_encodeChannelPrefix) == 0)
		{
			ChannelName = Configure::instance().m_encodeChannel;
			LOG(INFO) << "xml file [" << inputName << "], its channel [" <<  ChannelName << "] OK.";
			return true;
		}
		else
		{
			//LOG(INFO) << "xml file [" << inputName << "], its channel [" <<  ChannelName << "] is not we want, omit it.";
			return false;
		}
    }
    catch(std::exception& e)
    {
        LOG(ERROR) << "parse xml file error " << e.what();
        return false;
    }

    return true;



}
