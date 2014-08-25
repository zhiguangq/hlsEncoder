#ifndef NEWXMLFILE_H
#define NEWXMLFILE_H

#include <string>

#include "XMLInfo.h"

#include <Poco/DOM/Text.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Comment.h>
#include <Poco/DOM/ProcessingInstruction.h>
#include <Poco/DOM/Attr.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/XML/XMLWriter.h>
#include <Poco/FileStream.h>

class NewXMLFile
{
public:
    NewXMLFile(void);
    ~NewXMLFile(void);

    int archiveXML(std::string XMLFile,
               std::string AssetName,
               std::string EventName,
               std::string StartTime,
               std::string EndTime,
               std::string ChannelName);

    void GB2312ToUTF_8(std::string& pOut,char *pText, int pLen);
    void UTF_8ToGB2312(char*pOut, char *pText, int pLen);
private:
    void SubcontentInfoNode(Poco::XML::AutoPtr<Poco::XML::Document>& pDoc,
                Poco::XML::AutoPtr<Poco::XML::Element>& Content,
                std::string AssetName,
                std::string EventName,
                std::string StartTime,
                std::string EndTime,
                std::string ChannelName,
                std::string Rate,
                std::string Height,
                std::string Width,
                bool isM3u8);

    void UTF_8ToUnicode(wchar_t* pOut,char *pText);
    void UnicodeToUTF_8(char* pOut,wchar_t* pText);
    void UnicodeToGB2312(char* pOut,wchar_t uData);
    void Gb2312ToUnicode(wchar_t* pOut,char *gbBuffer);
};

#endif
