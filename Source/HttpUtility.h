#ifndef VCZH_HTTPUTILITY
#define VCZH_HTTPUTILITY

#include "String.h"
#include "Collections/Dictionary.h"

#ifdef VCZH_MSVC

namespace vl
{

/***********************************************************************
HTTP Utility
***********************************************************************/

	class HttpRequest
	{
		typedef collections::Array<char>					BodyBuffer;
		typedef collections::List<WString>					StringList;
		typedef collections::Dictionary<WString, WString>	HeaderMap;
	public:
		WString				server;
		vint				port;
		WString				query;
		bool				secure;
		WString				username;
		WString				password;
		WString				method;
		WString				cookie;
		BodyBuffer			body;
		WString				contentType;
		StringList			acceptTypes;
		HeaderMap			extraHeaders;

		HttpRequest();
		bool				SetHost(const WString& inputQuery);
		void				SetBodyUtf8(const WString& bodyString);
	};

	class HttpResponse
	{
		typedef collections::Array<char>		BodyBuffer;
	public:
		vint				statusCode;
		BodyBuffer			body;
		WString				cookie;

		HttpResponse();
		WString				GetBodyUtf8();
	};

	extern bool				HttpQuery(const HttpRequest& request, HttpResponse& response);
	extern WString			UrlEncodeQuery(const WString& query);
}

#endif

#endif
