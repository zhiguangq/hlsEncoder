#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <iostream>

class XMLInfo
{
public:
    XMLInfo(void);
    ~XMLInfo(void);

    bool load(std::string inputName);
public:
    std::string     AssetName;
    std::string     EventName;
    std::string     ChannelName;
    std::string     StartTime;
    std::string     EndTime;

};

#endif
