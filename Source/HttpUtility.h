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

	/// <summary>A type representing an http requiest.</summary>
	class HttpRequest
	{
		typedef collections::Array<char>					BodyBuffer;
		typedef collections::List<WString>					StringList;
		typedef collections::Dictionary<WString, WString>	HeaderMap;
	public:
		/// <summary>Name of the server, like "gaclib.net".</summary>
		WString				server;
		/// <summary>Port of the server, like 80.</summary>
		vint				port;
		/// <summary>Query of the request, like "/GettingStart.html".</summary>
		WString				query;
		/// <summary>Set to true if the request uses SSL.</summary>
		bool				secure;
		/// <summary>User name to authorize. Set to empty if you don't want to provide it.</summary>
		WString				username;
		/// <summary>Password to authorize. Set to empty if you don't want to provide it.</summary>
		WString				password;
		/// <summary>HTTP method, like "GET", "POST", "PUT", "DELETE", etc.</summary>
		WString				method;
		/// <summary>Cookie. Set to empty if you don't want to provide it.</summary>
		WString				cookie;
		/// <summary>Request body. This is a binary array using an array container to char.</summary>
		BodyBuffer			body;
		/// <summary>Content type, like "text/xml".</summary>
		WString				contentType;
		/// <summary>Accept type list, elements of it like "text/xml".</summary>
		StringList			acceptTypes;
		/// <summary>A dictionary to contain extra headers.</summary>
		HeaderMap			extraHeaders;

		/// <summary>Create an empty request.</summary>
		HttpRequest();

		/// <summary>Set <see cref="server"/>, <see cref="port"/>, <see cref="query"/> and <see cref="secure"/> fields for you using an URL.</summary>
		/// <returns>Returns true if this operation succeeded.</returns>
		/// <param name="inputQuery">The URL.</param>
		bool				SetHost(const WString& inputQuery);

		/// <summary>Fill the body with a text using UTF-8 encoding.</summary>
		/// <param name="bodyString">The text to fill.</param>
		void				SetBodyUtf8(const WString& bodyString);
	};
	
	/// <summary>A type representing an http response.</summary>
	class HttpResponse
	{
		typedef collections::Array<char>		BodyBuffer;
	public:
		/// <summary>Status code, like 200.</summary>
		vint				statusCode;
		/// <summary>Response body. This is a binary array using an array container to char.</summary>
		BodyBuffer			body;
		/// <summary>Returned cookie from the server.</summary>
		WString				cookie;

		HttpResponse();

		/// <summary>If you believe the server returns a text in UTF-8, use it to decode the body.</summary>
		/// <returns>The response body as text.</returns>
		WString				GetBodyUtf8();
	};

	/// <summary>Send an http request and receive a response.</summary>
	/// <returns>Returns true if this operation succeeded. Even the server returns 404 will be treated as success, because you get the response.</returns>
	/// <param name="request">The request.</param>
	/// <param name="response">The response.</param>
	extern bool				HttpQuery(const HttpRequest& request, HttpResponse& response);

	/// <summary>Encode a text as part of the url. This function can be used to create arguments in an URL.</summary>
	/// <returns>The encoded text.</returns>
	/// <param name="query">The text to encode.</param>
	extern WString			UrlEncodeQuery(const WString& query);
}

#endif

#endif
