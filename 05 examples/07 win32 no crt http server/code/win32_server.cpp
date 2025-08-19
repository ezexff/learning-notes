#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _NO_CRT_STDIO_INLINE

#include <windows.h>
#include <winsock2.h>
#include "engine_base.h"
#include "win32_winsock.h"

char HeaderHTML[] = 
"HTTP/1.0 200 OK\r\n"
"Connection: close\r\n"
"\r\n";
char BodyHTML[] = "<!DOCTYPE html><html><body><h1>My First Heading</h1><p>My first paragraph.</p></body></html>";

extern "C" int __stdcall mainCRTStartup(void)
{
    s32 Result = 0;
    
    InitConsole(); // TODO(ezexff):  mb remove?
    
	WSAData lpWSAData;
	WORD DLLVersion = MAKEWORD(2, 2);
    Result = WSAStartup(DLLVersion, &lpWSAData);
	if(Result != 0)
    {
        PrintLastError();
        WSACleanup();
        ConsolePause();
        ExitProcess(0);
	}
    
    sockaddr_in Addr = {};
	int sizeofaddr = sizeof(Addr);
	Addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	Addr.sin_port = htons(1111);
	Addr.sin_family = AF_INET;
    
	SOCKET ListenConnection = INVALID_SOCKET;
    ListenConnection = socket(AF_INET, SOCK_STREAM, 0);
    if(ListenConnection == INVALID_SOCKET)
    {
        PrintLastError();
        WSACleanup();
        ConsolePause();
        ExitProcess(0);
    }
    
	Result = bind(ListenConnection, (sockaddr *)&Addr, sizeof(Addr));
    if(Result == SOCKET_ERROR)
    {
        PrintLastError();
        WSACleanup();
        ConsolePause();
        ExitProcess(0);
    }
    
	Result = listen(ListenConnection, SOMAXCONN);
    if(Result == SOCKET_ERROR)
    {
        PrintLastError();
        WSACleanup();
        ConsolePause();
        ExitProcess(0);
    }
    
    printf("server %s:%d started\n", inet_ntoa(Addr.sin_addr), htons(Addr.sin_port));
    
	SOCKET Connection;
	while(true)
    {
		Connection = accept(ListenConnection, (sockaddr *)&Addr, &sizeofaddr);
		if(Connection  == INVALID_SOCKET)
        {
            PrintLastError();
            WSACleanup();
            ConsolePause();
            ExitProcess(0);
		}
        else
        {
            // NOTE(ezexff): recv
            char cCur = 0, cLast = 0, cBuf = 0;
            bool HeadersEnded = false;
            char ReqHdr[1024] = {0};
            int ReqHdrLen = 0;
            
            do
            {
                Result = recv(Connection, &cBuf, sizeof(cBuf), 0);
                if(Result <= 0)
                {
                    break;
                }
                
                // headers buf
                ReqHdr[ReqHdrLen++] = cBuf;
                
                // if finded headers end
                if(ReqHdrLen > 3 && ReqHdr[ReqHdrLen - 1] == '\n' && ReqHdr[ReqHdrLen - 2] == '\r' &&
                   ReqHdr[ReqHdrLen - 3] == '\n' && ReqHdr[ReqHdrLen - 4] == '\r')
                {
                    HeadersEnded = true;
                }
            } while (!HeadersEnded);
            
            // NOTE(ezexff): send
            char Page[1024];
            u64 Length = FormatString(sizeof(Page), Page, "%s%s", HeaderHTML, BodyHTML);
            send(Connection, Page, (int)Length, 0);
            
            closesocket(Connection);
            break;
		}
	}
    
    closesocket(ListenConnection);
    WSACleanup();
    
    ConsolePause();
    ExitProcess(0);
}