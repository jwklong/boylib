#pragma once

#include <string>

namespace Boy
{
	class HttpResponse
	{
	public:
		HttpResponse();
		virtual ~HttpResponse();

    public:
        std::string mBody;
        unsigned long mTimeStamp;
	};
}
