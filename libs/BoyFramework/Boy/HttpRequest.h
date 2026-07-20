#pragma once

#include <string>
#include <map>

namespace Boy
{
    class HttpResponseHandler;

	class HttpRequest
	{
	public:
		HttpRequest(const std::string &url, const std::string &host, HttpResponseHandler *handler);
        virtual ~HttpRequest();

        const std::string getParam(const std::string &key);
        const std::string getParamString();
        void setParam(const std::string &key, const std::string &value);
        void setParam(const std::string &key, int value);

        const std::string getURL();

    public:
        std::string mHost;
        std::string mResource;
        bool mUseHttps;
        int mTimeout;
        std::map<std::string, std::string> mParams;
        HttpResponseHandler *mHandler;
        std::string mReferrer;
        std::string mTag;
        unsigned long mTimeStamp;
	};
}
