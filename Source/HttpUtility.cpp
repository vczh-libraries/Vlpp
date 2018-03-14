#include "HttpUtility.h"
#include "Collections/OperationForEach.h"

#ifdef VCZH_MSVC
#include <Windows.h>
#include <winhttp.h>

#pragma comment(lib, "WinHttp.lib")

namespace vl
{
	using namespace collections;

/***********************************************************************
HttpRequest
***********************************************************************/

	HttpRequest::HttpRequest()
		:port(0)
		,secure(false)
	{
	}

	bool HttpRequest::SetHost(const WString& inputQuery)
	{
		server=L"";
		query=L"";
		port=0;
		secure=false;

		{
			if(server==L"")
			{
				if(inputQuery.Length()>7)
				{
					WString protocol=inputQuery.Sub(0, 8);
					if(_wcsicmp(protocol.Buffer(), L"https://")==0)
					{
						const wchar_t* reading=inputQuery.Buffer()+8;
						const wchar_t* index1=wcschr(reading, L':');
						const wchar_t* index2=wcschr(reading, L'/');
						if(index2)
						{
							query=index2;
							server=WString(reading, (index1?index1:index2)-reading);
							port=INTERNET_DEFAULT_HTTPS_PORT;
							secure=true;
							if(index1)
							{
								WString portString(index1+1, index2-index1-1);
								port=_wtoi(portString.Buffer());
							}
							return true;
						}
					}
				}
			}
			if(server==L"")
			{
				if(inputQuery.Length()>6)
				{
					WString protocol=inputQuery.Sub(0, 7);
					if(_wcsicmp(protocol.Buffer(), L"http://")==0)
					{
						const wchar_t* reading=inputQuery.Buffer()+7;
						const wchar_t* index1=wcschr(reading, L':');
						const wchar_t* index2=wcschr(reading, L'/');
						if(index2)
						{
							query=index2;
							server=WString(reading, (index1?index1:index2)-reading);
							port=INTERNET_DEFAULT_HTTP_PORT;
							if(index1)
							{
								WString portString(index1+1, index2-index1-1);
								port=_wtoi(portString.Buffer());
							}
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	void HttpRequest::SetBodyUtf8(const WString& bodyString)
	{
		vint utf8Size=WideCharToMultiByte(CP_UTF8, 0, bodyString.Buffer(), (int)bodyString.Length(), NULL, 0, NULL, NULL);
		char* utf8=new char[utf8Size+1];
		ZeroMemory(utf8, utf8Size+1);
		WideCharToMultiByte(CP_UTF8, 0, bodyString.Buffer(), (int)bodyString.Length(), utf8, (int)utf8Size, NULL, NULL);

		body.Resize(utf8Size);
		memcpy(&body[0], utf8, utf8Size);
		delete[] utf8;
	}

/***********************************************************************
HttpResponse
***********************************************************************/

	HttpResponse::HttpResponse()
		:statusCode(0)
	{
	}

	WString HttpResponse::GetBodyUtf8()
	{
		WString response;
		char* utf8=&body[0];
		vint totalSize=body.Count();
		vint utf16Size=MultiByteToWideChar(CP_UTF8, 0, utf8, (int)totalSize, NULL, 0);
		wchar_t* utf16=new wchar_t[utf16Size+1];
		ZeroMemory(utf16, (utf16Size+1)*sizeof(wchar_t));
		MultiByteToWideChar(CP_UTF8, 0, utf8, (int)totalSize, utf16, (int)utf16Size);
		response=utf16;
		delete[] utf16;
		return response;
	}

/***********************************************************************
Utilities
***********************************************************************/

	struct BufferPair
	{
		char*		buffer;
		vint		length;

		BufferPair()
			:buffer(0)
			,length(0)
		{
		}

		BufferPair(char* _buffer, vint _length)
			:buffer(_buffer)
			,length(_length)
		{
		}

		bool operator==(const BufferPair& pair){return false;}
		bool operator!=(const BufferPair& pair){return true;}
	};

	bool HttpQuery(const HttpRequest& request, HttpResponse& response)
	{
		// initialize
		response.statusCode=-1;
		HINTERNET internet=NULL;
		HINTERNET connectedInternet=NULL;
		HINTERNET requestInternet=NULL;
		BOOL httpResult=FALSE;
		DWORD error=0;
		List<LPCWSTR> acceptTypes;
		List<BufferPair> availableBuffers;

		// access http
		internet=WinHttpOpen(L"vczh", WINHTTP_ACCESS_TYPE_NO_PROXY, NULL, NULL, 0);
		error=GetLastError();
		if(!internet) goto CLEANUP;

		// connect
		connectedInternet=WinHttpConnect(internet, request.server.Buffer(), (int)request.port, 0);
		error=GetLastError();
		if(!connectedInternet) goto CLEANUP;

		// open request
		for(vint i=0;i<request.acceptTypes.Count();i++)
		{
			acceptTypes.Add(request.acceptTypes.Get(i).Buffer());
		}
		acceptTypes.Add(0);
		requestInternet=WinHttpOpenRequest(connectedInternet, request.method.Buffer(), request.query.Buffer(), NULL, WINHTTP_NO_REFERER, &acceptTypes[0], (request.secure?WINHTTP_FLAG_SECURE:0));
		error=GetLastError();
		if(!requestInternet) goto CLEANUP;

		// authentication, cookie and request
		if(request.username!=L"" && request.password!=L"")
		{
			WinHttpSetCredentials(requestInternet, WINHTTP_AUTH_TARGET_SERVER, WINHTTP_AUTH_SCHEME_BASIC, request.username.Buffer(), request.password.Buffer(), NULL);
		}
		if(request.contentType!=L"")
		{
			httpResult=WinHttpAddRequestHeaders(requestInternet, (L"Content-type:"+request.contentType).Buffer(), -1, WINHTTP_ADDREQ_FLAG_REPLACE|WINHTTP_ADDREQ_FLAG_ADD);
		}
		if(request.cookie!=L"")
		{
			WinHttpAddRequestHeaders(requestInternet, (L"Cookie:"+request.cookie).Buffer(), -1, WINHTTP_ADDREQ_FLAG_REPLACE|WINHTTP_ADDREQ_FLAG_ADD);
		}
		
		// extra headers
		for(int i=0;i<request.extraHeaders.Count();i++)
		{
			WString key=request.extraHeaders.Keys()[i];
			WString value=request.extraHeaders.Values().Get(i);
			WinHttpAddRequestHeaders(requestInternet, (key+L":"+value).Buffer(), -1, WINHTTP_ADDREQ_FLAG_REPLACE|WINHTTP_ADDREQ_FLAG_ADD);
		}

		if(request.body.Count()>0)
		{
			httpResult=WinHttpSendRequest(requestInternet, WINHTTP_NO_ADDITIONAL_HEADERS, 0, (LPVOID)&request.body.Get(0), (int)request.body.Count(), (int)request.body.Count(), NULL);
		}
		else
		{
			httpResult=WinHttpSendRequest(requestInternet, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, NULL);
		}
		error=GetLastError();
		if(httpResult==FALSE) goto CLEANUP;

		// receive response
		httpResult=WinHttpReceiveResponse(requestInternet, NULL);
		error=GetLastError();
		if(httpResult!=TRUE) goto CLEANUP;

		// read response status code
		{
			DWORD headerLength=sizeof(DWORD);
			DWORD statusCode=0;
			httpResult=WinHttpQueryHeaders(requestInternet, WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &headerLength, WINHTTP_NO_HEADER_INDEX);
			error=GetLastError();
			if(httpResult==FALSE) goto CLEANUP;
			response.statusCode=statusCode;
		}
		// read respons cookie
		{
			DWORD headerLength=sizeof(DWORD);
			httpResult=WinHttpQueryHeaders(requestInternet, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &headerLength, WINHTTP_NO_HEADER_INDEX);
			error=GetLastError();
			if(error==ERROR_INSUFFICIENT_BUFFER)
			{
				wchar_t* rawHeader=new wchar_t[headerLength/sizeof(wchar_t)];
				ZeroMemory(rawHeader, headerLength);
				httpResult=WinHttpQueryHeaders(requestInternet, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, rawHeader, &headerLength, WINHTTP_NO_HEADER_INDEX);
			
				const wchar_t* cookieStart=wcsstr(rawHeader, L"Cookie:");
				if(cookieStart)
				{
					const wchar_t* cookieEnd=wcsstr(cookieStart, L";");
					if(cookieEnd)
					{
						response.cookie=WString(cookieStart+7, cookieEnd-cookieStart-7);
					}
				}
				delete[] rawHeader;
			}
		}

		// read response body
		while(true)
		{
			DWORD bytesAvailable=0;
			BOOL queryDataAvailableResult=WinHttpQueryDataAvailable(requestInternet, &bytesAvailable);
			error=GetLastError();
			if(queryDataAvailableResult==TRUE && bytesAvailable!=0)
			{
				char* utf8=new char[bytesAvailable];
				DWORD bytesRead=0;
				BOOL readDataResult=WinHttpReadData(requestInternet, utf8, bytesAvailable, &bytesRead);
				error=GetLastError();
				if(readDataResult==TRUE)
				{
					availableBuffers.Add(BufferPair(utf8, bytesRead));
				}
				else
				{
					delete[] utf8;
				}
			}
			else
			{
				break;
			}
		}

		{
			// concatincate response body
			vint totalSize = 0;
			FOREACH(BufferPair, p, availableBuffers)
			{
				totalSize += p.length;
			}
			response.body.Resize(totalSize);
			if (totalSize > 0)
			{
				char* utf8 = new char[totalSize];
				{
					char* temp = utf8;
					FOREACH(BufferPair, p, availableBuffers)
					{
						memcpy(temp, p.buffer, p.length);
						temp += p.length;
					}
				}
				memcpy(&response.body[0], utf8, totalSize);
				delete[] utf8;
			}
			FOREACH(BufferPair, p, availableBuffers)
			{
				delete[] p.buffer;
			}
		}
	CLEANUP:
		if(requestInternet) WinHttpCloseHandle(requestInternet);
		if(connectedInternet) WinHttpCloseHandle(connectedInternet);
		if(internet) WinHttpCloseHandle(internet);
		return response.statusCode!=-1;
	}

	WString UrlEncodeQuery(const WString& query)
	{
		vint utf8Size=WideCharToMultiByte(CP_UTF8, 0, query.Buffer(),(int) query.Length(), NULL, 0, NULL, NULL);
		char* utf8=new char[utf8Size+1];
		ZeroMemory(utf8, utf8Size+1);
		WideCharToMultiByte(CP_UTF8, 0, query.Buffer(), (int)query.Length(), utf8, (int)utf8Size, NULL, NULL);

		wchar_t* encoded=new wchar_t[utf8Size*3+1];
		ZeroMemory(encoded, (utf8Size*3+1)*sizeof(wchar_t));
		wchar_t* writing=encoded;
		for(vint i=0;i<utf8Size;i++)
		{
			unsigned char x=(unsigned char)utf8[i];
			if(L'a'<=x && x<='z' || L'A'<=x && x<=L'Z' || L'0'<=x && x<=L'9')
			{
				writing[0]=x;
				writing+=1;
			}
			else
			{
				writing[0]=L'%';
				writing[1]=L"0123456789ABCDEF"[x/16];
				writing[2]=L"0123456789ABCDEF"[x%16];
				writing+=3;
			}
		}

		WString result=encoded;
		delete[] encoded;
		delete[] utf8;
		return result;
	}
}
#endif
