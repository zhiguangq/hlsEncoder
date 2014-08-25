#include <ocn/base/facility/log.h>

using namespace std;

namespace ocn
{
namespace base
{
namespace facility
{

string colon2dot(const string& name)
{
	string ret;
	bool isLastColon = false;
	int count = 0;
#ifdef _MSC_VER
	int i = 0;
#endif
	BOOST_FOREACH(char c, name)
	{
#ifdef _MSC_VER
		++i;
		if (i < 7)
			continue;
#endif

		if (c == ':')
		{
			if (count != 0)
			{
				ret.push_back(c);
				continue;
			}

			if (isLastColon)
			{
				isLastColon = false;
				ret.push_back('.');
			}
			else
			{
				isLastColon = true;
			}
		}
		else
		{
			if (c == '<')
			{
				++count;
			}
			else if (c == '>')
			{
				--count;
			}
			ret.push_back(c);
		}
	}
	return ret;
}

//
string demangling(const string& name)
{
	return Demangle::demangle(name);
}

}
}
}
