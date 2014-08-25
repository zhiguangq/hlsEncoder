/**
 * Title: base Json parser
 * Description: define commonly used for Json base communication.
 * Copyright: Copyright (c) 2013 ocn (All rights reserved)
 * Company: ocn
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-9-24
 * Version: 1.0
 * History: [1.0 2013-9-24]
 */
 
#ifndef JSON2_FACILITY_BASE_OCN_H
#define JSON2_FACILITY_BASE_OCN_H



#include <vector>
#include <utility>
#include <iterator>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include <json/json.h>
#include <json/json_object.h>
#include <json/json_tokener.h>

#ifdef __cplusplus
}
#endif

#define JSON_NAME_TOSTRING(name) #name

#define JSON_ADD_STRING(name) generic_json_parser_string(JSON_NAME_TOSTRING(name), name, m_object, m_curOper)
#define JSON_ADD_INT(name) generic_json_parser_int(JSON_NAME_TOSTRING(name), name, m_object, m_curOper)
#define JSON_ADD_OBJECT(name) generic_json_parser_object(JSON_NAME_TOSTRING(name), name, m_object, m_curOper)
#define JSON_ADD_OBJECT_STRING(name) generic_json_parser_object_string(JSON_NAME_TOSTRING(name), name, m_object, m_curOper)

namespace ocn
{
namespace base
{
namespace facility
{

enum OperType
{ 
	ToJson,  // Convert object to json string
	FromJson // Parse json string to object.
};

class Json
{
public:
	Json()
    {
    }

	virtual ~Json()
    {        
    }

	/**
	 * save json data to a string.
	 *
	 * @return The json data string.
	 */
	std::string toJson(void);

	/**
	 * save json data to a file.
	 *
	 * @return write file success or not
	 */
	bool toJson(const std::string& fileName);

	/**
	 * load json data from a string.
	 *
	 * @return str input json data string.
	 */
	bool fromJson(const std::string& str);

	/**
	 * load json data from a file.
	 *
	 * @return str input json data string.
	 */
    bool fromJsonFile(const std::string& fileName);

protected:   
    void generic_json_parser_string(const char* name, std::string& t, struct json_object* object, OperType oper)
    {
        if (FromJson == oper)
        {
            struct json_object* item = json_object_object_get(object, name);
			if (item != NULL)
			{
                t = json_object_get_string(json_object_object_get(object, name));
            }
        }
        else
        {
            if(!t.empty())
            {
                json_object_object_add(object, name, json_object_new_string(t.c_str()));
            }
        }
    }

    void generic_json_parser_int(const char* name, int& t, struct json_object* object, OperType oper)
    {
        if (FromJson == oper)
        {
            struct json_object* item = json_object_object_get(object, name);
			if (item != NULL)
			{
                t = json_object_get_int(json_object_object_get(object, name));
            }
        }
        else
        {
            json_object_object_add(object, name, json_object_new_int(t));
        }
    }

    template<typename T>
    void generic_json_parser_object(const char* name, std::vector<T>& t, struct json_object* object, OperType oper)
    {
        if (FromJson == oper)
        {
            struct json_object* arrayObject = json_object_object_get(m_object, name);
            for(int i=0; arrayObject != NULL && i < json_object_array_length(arrayObject); i++) 
            {
                json_object *obj = json_object_array_get_idx(arrayObject, i);
                T item;
                t.push_back(item);
                t.back().fromJson(json_object_to_json_string(obj));
            }
        }
        else if(ToJson == oper)
        { 
            struct json_object* arrayObject = json_object_new_array();
            for(int i = 0; i < (int)t.size(); i++) 
            {
                std::string str = t[i].toJson();
                struct json_object* item = json_tokener_parse(str.c_str());     
                json_object_array_add(arrayObject, item);
            }
            json_object_object_add(object, name, arrayObject);
        }
    }

    template<typename T>
    void generic_json_parser_object_string(const char* name, std::vector<T>& t, struct json_object* object, OperType oper)
    {
        if (FromJson == oper)
        {
            struct json_object* arrayObject = json_object_object_get(m_object, name);
            for(int i=0; arrayObject != NULL && i < json_object_array_length(arrayObject); i++) 
            {
                json_object *obj = json_object_array_get_idx(arrayObject, i);
                t.push_back(json_object_get_string(obj));
            }
        }
        else if(ToJson == oper)
        { 
            struct json_object* arrayObject = json_object_new_array();
            for(int i = 0; i < (int)t.size(); i++) 
            {
                T str = t[i];
                struct json_object* item = json_object_new_string(str.c_str());
                json_object_array_add(arrayObject, item);
            }
            json_object_object_add(object, name, arrayObject);
        }
    }

	OperType m_curOper;
    struct json_object*     m_object;

    virtual void assign() = 0;
};

} // facility
} // base
} // ocn

#endif 
