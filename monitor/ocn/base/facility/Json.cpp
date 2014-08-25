#include <ocn/base/facility/Json.h>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>

namespace ocn
{
namespace base
{
namespace facility
{

std::string Json::toJson()
{
	m_curOper = ToJson;
    m_object = json_object_new_object();
    assign();
    if(m_object != NULL && !is_error(m_object))
    {
        std::string ret = json_object_to_json_string(m_object);
        json_object_put(m_object);
        m_object = NULL;
        return ret;
    }
    return "{}";
}

bool Json::toJson(const std::string& fileName)
{
    boost::iostreams::stream_buffer<boost::iostreams::file_sink> buf(fileName);
    if(buf.is_open())
    {
        std::ostream   out(&buf);
        out << this->toJson();
    }
    else
    {
        return false;
    }
    return true;
}

bool Json::fromJson(const std::string& str)
{
	m_curOper = FromJson;
    m_object = json_tokener_parse(str.c_str());
    if(m_object != NULL && !is_error(m_object) && str[0] == '{')
    {
        assign();
        json_object_put(m_object);
        m_object = NULL;
        return true;
    }
    return false;
}

bool Json::fromJsonFile(const std::string& fileName)
{
    std::ifstream ifs(fileName.c_str());
    if(ifs)
    {        
        try
        {
            std::string jsonStr((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            return this->fromJson(jsonStr);
        }
        catch(...)
        {
        }        
    }

    return false;
}

} // facility
} // base
} // ocn
