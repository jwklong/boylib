#pragma once

#include <string>

namespace Boy
{
	class HttpResponse
	{
	public:
		HttpResponse() {}

    public:
        std::string mBody;
        unsigned long mTimeStamp;
	};
}
