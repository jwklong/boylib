#pragma once

#include "SDL2/SDL.h"
#include <vector>
#include <string>

namespace Boy
{
	class HttpRequest;
	class HttpResponse;

    size_t curlWriteFunction(char *ptr, size_t size, size_t nmemb, void *userdata);
    int httpQueueThreadProc(void *data);
    bool sendHttpRequest(HttpRequest *request, HttpResponse *response);

	class NetworkInterface
	{
	public:
		NetworkInterface();
        ~NetworkInterface();

        void httpQueueThreadProc();
        int processHttpRequest(HttpRequest *request);
        void queueHttpRequest(HttpRequest *request);
        bool sendHttpRequest(HttpRequest *request, HttpResponse *response);
        void setReplaceEnabled(const std::string &tag, bool enabled);
        void upload(const std::string &url, const std::string &filename, const std::string &content);

    public:
        static HttpResponse *gCurrentResponse;

    protected:
        SDL_mutex *mRequestProcessingMutex;
        SDL_mutex *mRequestQueueMutex;
        std::vector<HttpRequest*> mRequestQueue;
        bool mIsStopRequested;
        bool mIsThreadRunning;
        std::vector<std::string> mReplaceTags;
	};
}
