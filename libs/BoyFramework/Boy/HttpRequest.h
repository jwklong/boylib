#pragma once

#include <string>
#include <map>

namespace Boy
{
    class HttpResponseHandler;

	class HttpRequest
	{
	public:
		HttpRequest(const std::string &url, const std::string &unknown1, HttpResponseHandler *handler) {}

        const std::string getParam(std::string &buffer, const std::string &key);
        const std::string getParamString(std::string &buffer);
        void setParam(const std::string &key, int value);

        const std::string getURL(std::string &buffer);

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
