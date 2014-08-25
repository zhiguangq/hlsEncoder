#include "NewXMLFile.h"
#include "Configure.h"

#include <iostream>
#include "Poco/DateTime.h" 
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeParser.h"
#include "Poco/Timestamp.h"

using Poco::DateTime;
using Poco::Timespan;
using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::DateTimeParser;
using Poco::Timestamp;
using std::string;


NewXMLFile::NewXMLFile(void)
{
}


NewXMLFile::~NewXMLFile(void)
{
}

void NewXMLFile::UTF_8ToUnicode(wchar_t* pOut,char *pText)
{   
	char* uchar = (char *)pOut; 
	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F); 
} 

void NewXMLFile::UnicodeToUTF_8(char* pOut,wchar_t* pText) 
{
	// 注意 WCHAR高低字的顺序,低字节在前，高字节在后 
	char* pchar = (char *)pText; 
	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));
} 

void NewXMLFile::UnicodeToGB2312(char* pOut,wchar_t uData)
{ 
	WideCharToMultiByte(CP_ACP,NULL,&uData,1,pOut,sizeof(wchar_t),NULL,NULL); 
}

void NewXMLFile::Gb2312ToUnicode(wchar_t* pOut,char *gbBuffer) 
{   
	::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);
}

void NewXMLFile::GB2312ToUTF_8(string& pOut,char *pText, int pLen)
{  
	char buf[4] = {0}; 
	int nLength = pLen* 3; 
	char* rst = new char[nLength]; 	
	memset(rst,0,nLength);  
	int i = 0 ,j = 0; 
	while(i < pLen) 
	{ 
		//如果是英文直接复制就可以 
		if( *(pText + i) >= 0) 
		{  
			rst[j++] = pText[i++];
		} 
		else  
		{  
			wchar_t pbuffer;  
			Gb2312ToUnicode(&pbuffer,pText+i);
			UnicodeToUTF_8(buf,&pbuffer);
			rst[j] = buf[0];   
			rst[j+1] = buf[1]; 
			rst[j+2] = buf[2]; 
			j += 3; 
			i += 2; 
		}   
	}  

	//rst[j] ='\n';  	//返回结果  
	pOut = rst;
	delete []rst;
	return;   
}   

void NewXMLFile::UTF_8ToGB2312(char*pOut, char *pText, int pLen)
{   
	char Ctemp[4];   
	memset(Ctemp,0,4); 
	int i =0 ,j = 0; 
	while(i < pLen) 
	{  
		if(pText[i] >= 0)  
		{  
			pOut[j++] = pText[i++]; 
		} 
		else 
		{  
			WCHAR Wtemp; 
			UTF_8ToUnicode(&Wtemp,pText + i);
			UnicodeToGB2312(Ctemp,Wtemp); 
			pOut[j] = Ctemp[0];
			pOut[j + 1] = Ctemp[1];  
			i += 3;   
			j += 2;  
		}   
	} 
	//pOut[j] ='\n'; 
	return; 
} 

void NewXMLFile::SubcontentInfoNode(Poco::XML::AutoPtr<Poco::XML::Document>& pDoc,
    Poco::XML::AutoPtr<Poco::XML::Element>& Content,
    std::string AssetName,
    std::string EventName,
    std::string StartTime,
    std::string EndTime,
    std::string ChannelName,
    std::string Rate,
    std::string Height,
    std::string Width,
    bool isM3u8)
{
    Poco::XML::AutoPtr<Poco::XML::Element> SubcontentInfo  = pDoc->createElement("SubcontentInfo ");
        SubcontentInfo->setAttribute("subcontentType", "1");
        // 对应回看节目元数据中AssetName的前20字符
        SubcontentInfo->setAttribute("contentid", AssetName);
        Content->appendChild(SubcontentInfo);
    {
        Poco::XML::AutoPtr<Poco::XML::Element> SubcontentBaseInfo = pDoc->createElement("SubcontentBaseInfo");
            SubcontentInfo->appendChild(SubcontentBaseInfo);
        {
            Poco::XML::AutoPtr<Poco::XML::Element> SubcontentName = pDoc->createElement("SubcontentName");
                // 对应回看节目元数据中EventName字段
                SubcontentName->appendChild(pDoc->createTextNode(EventName));
                SubcontentBaseInfo->appendChild(SubcontentName);
            Poco::XML::AutoPtr<Poco::XML::Element> SubcontentSize = pDoc->createElement("SubcontentSize");
                // mp4或m3u8字节数
                //SubcontentSize->appendChild(pDoc->createTextNode(isM3u8 ? "m3u8" : "mp4"));
                SubcontentBaseInfo->appendChild(SubcontentSize);
            Poco::XML::AutoPtr<Poco::XML::Element> SubcontentFormat = pDoc->createElement("SubcontentFormat");
                SubcontentFormat->appendChild(pDoc->createTextNode("1"));
                SubcontentBaseInfo->appendChild(SubcontentFormat);
            Poco::XML::AutoPtr<Poco::XML::Element> GetMethod = pDoc->createElement("GetMethod");
                GetMethod->appendChild(pDoc->createTextNode("0"));
                SubcontentBaseInfo->appendChild(GetMethod);
            Poco::XML::AutoPtr<Poco::XML::Element> SubcontentURL = pDoc->createElement("SubcontentURL");
                // 文件最终路径，此处可以再商量下，用file形式，还是http形式
                if(isM3u8)
                {
                    SubcontentURL->appendChild(pDoc->createTextNode(AssetName+ "\\" + Rate + "\\" + Rate + ".m3u8"));
                }
                else
                {
                    SubcontentURL->appendChild(pDoc->createTextNode(AssetName+ "\\" + Rate + ".mp4"));
                }
                SubcontentBaseInfo->appendChild(SubcontentURL);
            Poco::XML::AutoPtr<Poco::XML::Element> Description = pDoc->createElement("Description");
                // 对应回看节目元数据中EventName字段
                Description->appendChild(pDoc->createTextNode(EventName));
                SubcontentBaseInfo->appendChild(Description);
            Poco::XML::AutoPtr<Poco::XML::Element> MD5Info = pDoc->createElement("MD5Info");
                // 对文件进行MD5计算
                //MD5Info->appendChild(pDoc->createTextNode("???"));
                SubcontentBaseInfo->appendChild(MD5Info);
            Poco::XML::AutoPtr<Poco::XML::Element> MimeType = pDoc->createElement("MimeType");
                // 对m3u8类型固定取值为45, mp4类型为43
                MimeType->appendChild(pDoc->createTextNode(isM3u8 ? "45" : "43"));
                SubcontentBaseInfo->appendChild(MimeType);
            Poco::XML::AutoPtr<Poco::XML::Element> ModelList = pDoc->createElement("ModelList");
                // 节目适用的终端，m3u8类型写"iOS，Android"，  Mp4文件类型写"Andorid"
                ModelList->appendChild(pDoc->createTextNode(isM3u8 ? "iOS,Android" : "Andorid"));
                SubcontentBaseInfo->appendChild(ModelList);
            Poco::XML::AutoPtr<Poco::XML::Element> ReplayTVStartTime = pDoc->createElement("ReplayTVStartTime");
                // 用于指明回看节目的播出开始StartTime
                ReplayTVStartTime->appendChild(pDoc->createTextNode(StartTime));
                SubcontentBaseInfo->appendChild(ReplayTVStartTime);
            Poco::XML::AutoPtr<Poco::XML::Element> ReplayTVEndTime = pDoc->createElement("ReplayTVEndTime");
                // 用于指明回看节目的结束时间EndTime
                ReplayTVEndTime->appendChild(pDoc->createTextNode(EndTime));
                SubcontentBaseInfo->appendChild(ReplayTVEndTime);
            Poco::XML::AutoPtr<Poco::XML::Element> ReplayTVChannel = pDoc->createElement("ReplayTVChannel");
                // 新增字段，用于指明回看节目所在频道
                ReplayTVChannel->appendChild(pDoc->createTextNode(ChannelName));
                SubcontentBaseInfo->appendChild(ReplayTVChannel);
            Poco::XML::AutoPtr<Poco::XML::Element> ChannelID = pDoc->createElement("ChannelId");
                // 新增字段，用于指明回看节目所在频道ID
                ChannelID->appendChild(pDoc->createTextNode(Configure::instance().m_encodeChannelID));
                SubcontentBaseInfo->appendChild(ChannelID);
            Poco::XML::AutoPtr<Poco::XML::Element> ServiceType = pDoc->createElement("ServiceType");
                ServiceType->appendChild(pDoc->createTextNode("0"));
                SubcontentBaseInfo->appendChild(ServiceType);
        }
        Poco::XML::AutoPtr<Poco::XML::Element> SubcontentCopyrights = pDoc->createElement("SubcontentCopyrights");
            SubcontentInfo->appendChild(SubcontentCopyrights);
        {
            Poco::XML::AutoPtr<Poco::XML::Element> CopyrightInfo = pDoc->createElement("CopyrightInfo");
            SubcontentCopyrights->appendChild(CopyrightInfo);
        {
            Poco::XML::AutoPtr<Poco::XML::Element> Type = pDoc->createElement("Type");
                Type->appendChild(pDoc->createTextNode("1"));
                CopyrightInfo->appendChild(Type);
            Poco::XML::AutoPtr<Poco::XML::Element> EffectiveTime = pDoc->createElement("EffectiveTime");
                // 节目生效时间，节目转码完成的当天
                {
                    DateTime currentTime;
                    EffectiveTime->appendChild(pDoc->createTextNode(DateTimeFormatter::format(currentTime, "%Y-%m-%d")));
                }
                CopyrightInfo->appendChild(EffectiveTime);
            Poco::XML::AutoPtr<Poco::XML::Element> ExpireTime = pDoc->createElement("ExpireTime");
                // 节目时效时间，EffectiveTime + 有效天数 ，每个频道的有效天数都需要能单独设置 (暂时设定3天)
                {
                    DateTime currentTime;
                    Timestamp next3date(currentTime.timestamp().epochMicroseconds() + 259200000000LL);
                    DateTime nextTime(next3date);
                    ExpireTime->appendChild(pDoc->createTextNode(DateTimeFormatter::format(nextTime, "%Y-%m-%d")));
                }
                CopyrightInfo->appendChild(ExpireTime);
            Poco::XML::AutoPtr<Poco::XML::Element> AllowedArea = pDoc->createElement("AllowedArea");
                // (固定”上海”)
                {
                    std::string text = "上海";
                    std::string utf8Text;
                    char buf[128];
                    std::strcpy(buf, text.c_str());
                    GB2312ToUTF_8(utf8Text, buf, text.size());
                    AllowedArea->appendChild(pDoc->createTextNode(utf8Text));
                }
                CopyrightInfo->appendChild(AllowedArea);
        }
        }
        Poco::XML::AutoPtr<Poco::XML::Element> SubcontentVideoInfo = pDoc->createElement("SubcontentVideoInfo");
            SubcontentInfo->appendChild(SubcontentVideoInfo);
        {
            Poco::XML::AutoPtr<Poco::XML::Element> Duration = pDoc->createElement("Duration");
                // 节目时长，根据输入元数据的StartTime与EndTime计算后得到
                {
                    int tzd;
                    DateTime end = DateTimeParser::parse(DateTimeFormat::SORTABLE_FORMAT, EndTime, tzd);
                    DateTime start = DateTimeParser::parse(DateTimeFormat::SORTABLE_FORMAT, StartTime, tzd);
                    Poco::Timespan span(end - start);
                    Duration->appendChild(pDoc->createTextNode(DateTimeFormatter::format(span, "%H:%M:%S")));
                }
                SubcontentVideoInfo->appendChild(Duration);
            Poco::XML::AutoPtr<Poco::XML::Element> BitRate = pDoc->createElement("BitRate");
                BitRate->appendChild(pDoc->createTextNode(Rate));
                SubcontentVideoInfo->appendChild(BitRate);
            Poco::XML::AutoPtr<Poco::XML::Element> FrameHeight = pDoc->createElement("FrameHeight");
                FrameHeight->appendChild(pDoc->createTextNode(Height));
                SubcontentVideoInfo->appendChild(FrameHeight);
            Poco::XML::AutoPtr<Poco::XML::Element> FrameWidth = pDoc->createElement("FrameWidth");
                FrameWidth->appendChild(pDoc->createTextNode(Width ));
                SubcontentVideoInfo->appendChild(FrameWidth);
            Poco::XML::AutoPtr<Poco::XML::Element> AspectRatio = pDoc->createElement("AspectRatio");
                AspectRatio->appendChild(pDoc->createTextNode("4:3"));
                SubcontentVideoInfo->appendChild(AspectRatio);
            Poco::XML::AutoPtr<Poco::XML::Element> FrameRate = pDoc->createElement("FrameRate");
                FrameRate->appendChild(pDoc->createTextNode("25"));
                SubcontentVideoInfo->appendChild(FrameRate);
            Poco::XML::AutoPtr<Poco::XML::Element> Brightness = pDoc->createElement("Brightness");
                Brightness->appendChild(pDoc->createTextNode("50"));
                SubcontentVideoInfo->appendChild(Brightness);
            Poco::XML::AutoPtr<Poco::XML::Element> Contrast = pDoc->createElement("Contrast");
                Contrast->appendChild(pDoc->createTextNode("50:1"));
                SubcontentVideoInfo->appendChild(Contrast);
            Poco::XML::AutoPtr<Poco::XML::Element> Loudness = pDoc->createElement("Loudness");
                Loudness->appendChild(pDoc->createTextNode("50"));
                SubcontentVideoInfo->appendChild(Loudness);
            Poco::XML::AutoPtr<Poco::XML::Element> VideoCodec = pDoc->createElement("VideoCodec");
                VideoCodec->appendChild(pDoc->createTextNode("H.264"));
                SubcontentVideoInfo->appendChild(VideoCodec);
            Poco::XML::AutoPtr<Poco::XML::Element> AudioCodec = pDoc->createElement("AudioCodec");
                AudioCodec->appendChild(pDoc->createTextNode("AAC"));
                SubcontentVideoInfo->appendChild(AudioCodec);
            Poco::XML::AutoPtr<Poco::XML::Element> HDFlag = pDoc->createElement("HDFlag");
                // 节目转码参数，此处样为标清，高清需要按1280x720填写，且高清情况下HDFlag需要置为1
                HDFlag->appendChild(pDoc->createTextNode("0"));
                SubcontentVideoInfo->appendChild(HDFlag);
        }
        Poco::XML::AutoPtr<Poco::XML::Element> SubcontentExtendInfo = pDoc->createElement("SubcontentExtendInfo");
            SubcontentInfo->appendChild(SubcontentExtendInfo);
    }
}

int NewXMLFile::archiveXML(std::string XMLFile,
               std::string AssetName,
               std::string EventName,
               std::string StartTime,
               std::string EndTime,
               std::string ChannelName)
{
	Poco::XML::AutoPtr<Poco::XML::Document> pDoc = new Poco::XML::Document;
	Poco::XML::AutoPtr<Poco::XML::ProcessingInstruction> pi = pDoc->createProcessingInstruction("xml","version='1.0' encoding='UTF-8'" );

    Poco::XML::AutoPtr<Poco::XML::Element> Message = pDoc->createElement("Message");
    {
        Message->setAttribute("streamingNO", "13874466127661");
        Message->setAttribute("providerID", "www.sgrand.cn");
        Message->setAttribute("receiverID", "www.inesanet.com");
        {
            DateTime currentTime;
            Message->setAttribute("timestamp", DateTimeFormatter::format(currentTime, DateTimeFormat::ISO8601_FORMAT));
        }
        Message->setAttribute("opFlag", "1");

        {
            {
                Poco::XML::AutoPtr<Poco::XML::Element> Content = pDoc->createElement("Content");
                    Content->setAttribute("contentClass", "rtv");
                    // 对应回看节目元数据中AssetName的前20字符,对应简版:AssetName="CVHM1317418470001800.ts
                    Content->setAttribute("objectContentID", AssetName);
                    Message->appendChild(Content);
                {
                    Poco::XML::AutoPtr<Poco::XML::Element> ObjectContentInfo = pDoc->createElement("ObjectContentInfo");
                        Content->appendChild(ObjectContentInfo);
                    {
                        Poco::XML::AutoPtr<Poco::XML::Element> ContentBaseInfo = pDoc->createElement("ContentBaseInfo");
                            ObjectContentInfo->appendChild(ContentBaseInfo);
                        {
                            Poco::XML::AutoPtr<Poco::XML::Element> Title = pDoc->createElement("Title");
                                ContentBaseInfo->appendChild(Title);
                            {
                                Poco::XML::AutoPtr<Poco::XML::Element> ProperTitle = pDoc->createElement("ProperTitle");
                                    // 对应回看节目元数据中EventName字段
                                    ProperTitle->appendChild(pDoc->createTextNode(EventName));
                                    Title->appendChild(ProperTitle);
                                Poco::XML::AutoPtr<Poco::XML::Element> ParallelProperTitle = pDoc->createElement("ParallelProperTitle");
                                    // 对应回看节目元数据中EventName字段
                                    ParallelProperTitle->appendChild(pDoc->createTextNode(EventName));
                                    Title->appendChild(ParallelProperTitle);
                            }
                            Poco::XML::AutoPtr<Poco::XML::Element> ContentVersion = pDoc->createElement("ContentVersion");
                                ContentVersion->appendChild(pDoc->createTextNode("1"));
                                ContentBaseInfo->appendChild(ContentVersion);
                            Poco::XML::AutoPtr<Poco::XML::Element> CPID = pDoc->createElement("CPID");
                                CPID->appendChild(pDoc->createTextNode("OCN"));
                                ContentBaseInfo->appendChild(CPID);
                            Poco::XML::AutoPtr<Poco::XML::Element> CPType = pDoc->createElement("CPType");
                                CPType->appendChild(pDoc->createTextNode("4"));
                                ContentBaseInfo->appendChild(CPType);
                            Poco::XML::AutoPtr<Poco::XML::Element> Tag = pDoc->createElement("Tag");
                                {
                                    // 固定为"云视回看"
                                    std::string text = "云视回看";
                                    std::string utf8Text;
                                    char buf[128];
                                    std::strcpy(buf, text.c_str());
                                    GB2312ToUTF_8(utf8Text, buf, text.size());
                                    Tag->appendChild(pDoc->createTextNode(utf8Text));
                                }
                                ContentBaseInfo->appendChild(Tag);
                            Poco::XML::AutoPtr<Poco::XML::Element> IsPublicFlag = pDoc->createElement("IsPublicFlag");
                                IsPublicFlag->appendChild(pDoc->createTextNode("0"));
                                ContentBaseInfo->appendChild(IsPublicFlag);
                            Poco::XML::AutoPtr<Poco::XML::Element> IsAdvertiseFlag = pDoc->createElement("IsAdvertiseFlag");
                                IsAdvertiseFlag->appendChild(pDoc->createTextNode("0"));
                                ContentBaseInfo->appendChild(IsAdvertiseFlag);
                            Poco::XML::AutoPtr<Poco::XML::Element> NeedDRMPack = pDoc->createElement("NeedDRMPack");
                                NeedDRMPack->appendChild(pDoc->createTextNode("0"));
                                ContentBaseInfo->appendChild(NeedDRMPack);
                            Poco::XML::AutoPtr<Poco::XML::Element> DRMDescFileURL = pDoc->createElement("DRMDescFileURL");  
                                DRMDescFileURL->appendChild(pDoc->createTextNode(""));
                                ContentBaseInfo->appendChild(DRMDescFileURL);
                        }

                        Poco::XML::AutoPtr<Poco::XML::Element> ContentVideoInfo  = pDoc->createElement("ContentVideoInfo ");
                            ContentVideoInfo->setAttribute("parentID", "");
                            ContentVideoInfo->setAttribute("videoType", "11");
                            ObjectContentInfo->appendChild(ContentVideoInfo);
                        {
                            Poco::XML::AutoPtr<Poco::XML::Element> VideoBaseInfo = pDoc->createElement("VideoBaseInfo");
                                ContentVideoInfo->appendChild(VideoBaseInfo);
                            {
                                Poco::XML::AutoPtr<Poco::XML::Element> Category1 = pDoc->createElement("Category1");
                                    {
                                        // 固定为"云视回看"
                                        std::string text = "云视回看";
                                        std::string utf8Text;
                                        char buf[128];
                                        std::strcpy(buf, text.c_str());
                                        GB2312ToUTF_8(utf8Text, buf, text.size());
                                        Category1->appendChild(pDoc->createTextNode(utf8Text));  // to utf8
                                    }
                                    VideoBaseInfo->appendChild(Category1);
                                Poco::XML::AutoPtr<Poco::XML::Element> Category2 = pDoc->createElement("Category2");
                                    Category2->appendChild(pDoc->createTextNode(""));
                                    VideoBaseInfo->appendChild(Category2);
                                Poco::XML::AutoPtr<Poco::XML::Element> CollectionCount = pDoc->createElement("CollectionCount");
                                    CollectionCount->appendChild(pDoc->createTextNode("1"));
                                    VideoBaseInfo->appendChild(CollectionCount);
                                Poco::XML::AutoPtr<Poco::XML::Element> OrderNumber = pDoc->createElement("OrderNumber");
                                    OrderNumber->appendChild(pDoc->createTextNode("1"));
                                    VideoBaseInfo->appendChild(OrderNumber);
                                Poco::XML::AutoPtr<Poco::XML::Element> PublishDate = pDoc->createElement("PublishDate");
                                    // 此处写输入元数据中的StartTime的日期
                                    {
                                        DateTime currentTime;
                                        PublishDate->appendChild(pDoc->createTextNode(DateTimeFormatter::format(currentTime, "%Y-%m-%d")));
                                    }
                                    VideoBaseInfo->appendChild(PublishDate);
                                Poco::XML::AutoPtr<Poco::XML::Element> PublishArea = pDoc->createElement("PublishArea");
                                    PublishArea->appendChild(pDoc->createTextNode("1"));
                                    VideoBaseInfo->appendChild(PublishArea);
                                Poco::XML::AutoPtr<Poco::XML::Element> AudioLanguage = pDoc->createElement("AudioLanguage");
                                    {
                                        // 固定为"中文"
                                        std::string text = "中文";
                                        std::string utf8Text;
                                        char buf[128];
                                        std::strcpy(buf, text.c_str());
                                        GB2312ToUTF_8(utf8Text, buf, text.size());
                                        AudioLanguage->appendChild(pDoc->createTextNode(utf8Text)); // to utf8
                                    }
                                    VideoBaseInfo->appendChild(AudioLanguage);
                                Poco::XML::AutoPtr<Poco::XML::Element> Description = pDoc->createElement("Description");
                                    // 对应回看节目元数据中EventName字段
                                    Description->appendChild(pDoc->createTextNode(EventName));
                                    VideoBaseInfo->appendChild(Description);
                            }
                        }
                    }

                    // 四个 SubcontentInfo 
                    SubcontentInfoNode(pDoc, Content, AssetName, EventName, StartTime, EndTime, ChannelName, "1200k", "1280", "720", true);
                    SubcontentInfoNode(pDoc, Content, AssetName, EventName, StartTime, EndTime, ChannelName, "800k", "720", "576", true);
                    SubcontentInfoNode(pDoc, Content, AssetName, EventName, StartTime, EndTime, ChannelName, "400k", "480", "320", true);
                    SubcontentInfoNode(pDoc, Content, AssetName, EventName, StartTime, EndTime, ChannelName, "1200k", "1280", "720", false);
                }
            }
        }
    }
  
	pDoc->appendChild(pi);	
	pDoc->appendChild(Message);

	Poco::XML::DOMWriter write;
	write.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
	Poco::FileStream ofs(XMLFile, std::ios::in);
	write.writeNode(ofs,pDoc);

	return 0;
}

