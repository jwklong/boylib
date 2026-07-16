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
        int processHttpRequest(HttpRequest *request);
        void queueHttpRequest(HttpRequest *request);
        void sendHttpRequest(HttpRequest *request, HttpResponse *response);
        void setReplaceEnabled(const std::string &tag, bool enabled);
        void upload(const std::string &unknown1, const std::string &unknown2, const std::string &unknown3);

    protected:
        SDL_mutex *mRequestProcessingMutex;
        SDL_mutex *mRequestQueueMutex;
        std::vector<HttpRequest*> mRequestQueue;
        bool mIsStopRequested;
        bool mIsThreadRunning;
        std::vector<std::string> mReplaceTags;
	};

    void sendHttpRequest(HttpRequest *request, HttpResponse *response);
}
