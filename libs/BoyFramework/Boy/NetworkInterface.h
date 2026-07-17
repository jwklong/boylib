#pragma once

#include <SDL/SDL.h>
#include <vector>
#include <string>

namespace Boy
{
	class HttpRequest;
	class HttpResponse;

	class NetworkInterface
	{
	public:
		NetworkInterface() {}

        void httpQueueThreadProc();
        int processHttpRequest(HttpRequest *request, void *userp);
        void queueHttpRequest(HttpRequest *request);
        void sendHttpRequest(HttpRequest *request, HttpResponse *response);
        void setReplaceEnabled(const std::string &tag, bool enabled);
        void upload(const std::string &url, const std::string &filename, const std::string &content);

    protected:
        SDL_mutex *mRequestProcessingMutex;
        SDL_mutex *mRequestQueueMutex;
        std::vector<HttpRequest*> mRequestQueue;
        bool mIsStopRequested;
        bool mIsThreadRunning;
        std::vector<std::string> mReplaceTags;
	};

    void curlWriteFunction(void *contents, unsigned int size, unsigned int nmemb, void *userp);
    void httpQueueThreadProc(void *unknown1);
    void sendHttpRequest(HttpRequest *request, HttpResponse *response);
}
