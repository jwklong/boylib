#include "HttpRequest.h"

#include "Util.h"

using namespace Boy;

HttpRequest::HttpRequest(const std::string &url, const std::string &host, HttpResponseHandler *handler)
{
    mHost = host;
    mHandler = handler;
    mUseHttps = false;
    mTimeout = 0;

    size_t queryPos = url.find_first_of('?');
    
    if (queryPos == std::string::npos)
    {
        mResource = url;
    }
    else
    {
        std::vector<std::string> tokens;
        tokenize(url, "?&=", tokens);

        if (!tokens.empty())
        {
            mResource = tokens[0];
        }

        for (size_t i = 1; i + 1 < tokens.size(); i += 2)
        {
            mParams[tokens[i]] = tokens[i + 1];
        }
    }
}

HttpRequest::~HttpRequest()
{
    
}

const std::string HttpRequest::getParam(const std::string &key)
{
	std::map<std::string,std::string>::iterator iter = mParams.find(key);
	if (iter!=mParams.end())
	{
		return iter->second;
	}
	else
	{
		return std::string("");
	}
}

const std::string HttpRequest::getParamString()
{
    std::string result;

    std::map<std::string,std::string>::iterator iter;
    for (iter=mParams.begin() ; iter!=mParams.end() ; iter++)
	{
		if (iter!=mParams.begin())
        {
            result += "&";
        }
        result += iter->first;
        result += "=";
        result += iter->second;
	}

    return result;
}

void HttpRequest::setParam(const std::string &key, const std::string &value)
{
    mParams[key] = value;
}

void HttpRequest::setParam(const std::string &key, int value)
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", value);
    mParams[key] = buffer;
}

const std::string HttpRequest::getURL()
{
    std::string result = this->mUseHttps ? "https://" : "http://";
    result.append(this->mHost);
    if (result.back() != '/' && this->mResource.front() != '/') result.append("/"); // ensure only one slash between host & resource
    result.append(this->mResource);
    return result;
}