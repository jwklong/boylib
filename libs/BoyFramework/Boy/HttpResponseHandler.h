#pragma once

namespace Boy
{
	class HttpRequest;
	class HttpResponse;

	class HttpResponseHandler
	{
	public:
		HttpResponseHandler() {}
		virtual ~HttpResponseHandler() {}

		virtual void handleMessage(HttpRequest *request, HttpResponse *response) = 0;
	};
}
