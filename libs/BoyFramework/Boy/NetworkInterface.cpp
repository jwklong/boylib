#include "NetworkInterface.h"

#include <algorithm>
#include "curl/curl.h"
#include "Environment.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpResponseHandler.h"

using namespace Boy;

size_t Boy::curlWriteFunction(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t totalSize = size * nmemb;
    if (totalSize == 0)
    {
        return 0;
    }

    NetworkInterface::gCurrentResponse->mBody.append(ptr, totalSize);

    return totalSize;
}

int Boy::httpQueueThreadProc(void *data)
{
    Environment::instance()->getNetworkInterface()->httpQueueThreadProc();
    return 0;
}

bool Boy::sendHttpRequest(HttpRequest *request, HttpResponse *response)
{
    envDebugLog("HTTP POST: %s?%s\n", request->getURL().c_str(), request->getParamString().c_str());

    CURL *curl = curl_easy_init();
    if (curl == NULL)
    {
        return false;
    }

    NetworkInterface::gCurrentResponse = response;

    curl_easy_setopt(curl, CURLOPT_URL, request->getURL().c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    std::string postData = request->getParamString();
    if (!postData.empty())
    {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    }
    if (!request->mUseHttps)
    {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }
    if (request->mTimeout > 0)
    {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)request->mTimeout);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    }
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Boy::curlWriteFunction);

    unsigned long startTime = clock();
    request->mTimeStamp = startTime;

    CURLcode res = curl_easy_perform(curl);

    NetworkInterface::gCurrentResponse = NULL;
    unsigned long endTime = static_cast<unsigned long>(clock());
    response->mTimeStamp = endTime;

    curl_easy_cleanup(curl);

    envDebugLog("HTTP response: %s (t=%0.2f)\n", response->mBody.c_str(), (double)(endTime - startTime) / CLOCKS_PER_SEC);

    return res == CURLE_OK;
}

static HttpResponse sCurrentResponse;
HttpResponse *NetworkInterface::gCurrentResponse = &sCurrentResponse;

NetworkInterface::NetworkInterface()
{
    mRequestProcessingMutex = SDL_CreateMutex();
    mRequestQueueMutex = SDL_CreateMutex();
    curl_global_init(CURL_GLOBAL_ALL);
}

NetworkInterface::~NetworkInterface()
{
    mIsStopRequested = true;
    while (mIsThreadRunning)
    {
        SDL_Delay(10);
    }

    SDL_DestroyMutex(mRequestProcessingMutex);
    SDL_DestroyMutex(mRequestQueueMutex);
    curl_global_cleanup();
}

void NetworkInterface::httpQueueThreadProc()
{
    envDebugLog("*** COMM THREAD STARTING ***\n");

    while (!mIsStopRequested)
    {
        SDL_LockMutex(mRequestQueueMutex);
        HttpRequest *request = NULL;
        if (!mRequestQueue.empty()) {
            request = mRequestQueue.front();
            mRequestQueue.erase(mRequestQueue.begin());
        }
        SDL_UnlockMutex(mRequestQueueMutex);

        if (request != NULL)
        {
            processHttpRequest(request);
        }
        else
        {
            SDL_Delay(100);
        }
    }

    envDebugLog("*** COMM THREAD ENDING ***\n");
    mIsThreadRunning = false;
}

int NetworkInterface::processHttpRequest(HttpRequest *request)
{
    HttpResponse *response;
    sendHttpRequest(request, response);
    if (request->mHandler != NULL)
    {
        request->mHandler->handleMessage(request, response);
    }

    return 0;
}

void NetworkInterface::queueHttpRequest(HttpRequest *request)
{
    if (!mIsThreadRunning)
    {
        SDL_CreateThread(Boy::httpQueueThreadProc, NULL, NULL);
        mIsThreadRunning = true;
        SDL_LockMutex(mRequestQueueMutex);

        //find duplicates of the same tag
        if (!request->mTag.empty())
        {
            for (size_t i = 0; i < mRequestQueue.size(); ++i)
            {
                HttpRequest* existingRequest = mRequestQueue[i];
                if (existingRequest->mTag == request->mTag)
                {
                    envDebugLog("NET: replacing request (tag=%s)\n", request->mTag.c_str());
                    delete existingRequest;
                    mRequestQueue[i] = request;
                    SDL_UnlockMutex(mRequestQueueMutex);
                    return;
                }
            }
        }

        mRequestQueue.push_back(request);
        SDL_UnlockMutex(mRequestQueueMutex);
    }
}

bool NetworkInterface::sendHttpRequest(HttpRequest *request, HttpResponse *response)
{
    SDL_LockMutex(mRequestProcessingMutex);
    bool result = Boy::sendHttpRequest(request, response);
    SDL_UnlockMutex(mRequestProcessingMutex);
    return result;
}

void NetworkInterface::setReplaceEnabled(const std::string &tag, bool enabled)
{
    std::vector<std::string>::iterator iter = find(mReplaceTags.begin(), mReplaceTags.end(), tag);
    if (enabled)
    {
        if (iter == mReplaceTags.end())
        {
            mReplaceTags.push_back(tag);
        }
    }
    else
    {
        if (iter != mReplaceTags.end())
        {
            mReplaceTags.erase(iter);
        }
    }
}

void NetworkInterface::upload(const std::string &url, const std::string &filename, const std::string &content)
{
    curl_httppost* formPost = NULL;
    curl_httppost* lastPtr = NULL;
    curl_formadd(&formPost, &lastPtr,
        CURLFORM_COPYNAME, "file",
        CURLFORM_FILE, filename.c_str(),
        CURLFORM_COPYCONTENTS, content.c_str(),
        CURLFORM_CONTENTTYPE, "text/plain",
        CURLFORM_END);

    CURL* curl = curl_easy_init();
    
    // no idea what this does but its in the code so
    curl_slist* customHeaders = curl_slist_append(nullptr, "Expect:");

    if (curl != nullptr)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formPost);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, customHeaders);

        curl_easy_perform(curl);

        curl_easy_cleanup(curl);
        curl_formfree(formPost);
        curl_slist_free_all(customHeaders);
    }
}