#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _NO_CRT_STDIO_INLINE

#if (!APP_NO_CRT)
#include <stdio.h>
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif

#include <windows.h>
#include <winsock2.h>
#include "engine_base.h"
#include "win32_winsock.h"

struct connection_info
{
    b32 IsInitialized;
    
    u32 TmpIndex; // TODO(ezexff): mb remove
    u32 ThreadID;
    sockaddr_in Addr;
    SOCKET Socket;
};

#define MAX_CONNECTION_COUNT 3
connection_info ConnectionInfoArray[MAX_CONNECTION_COUNT];

DWORD WINAPI
ThreadProcConnection(LPVOID lpParameter)
{
    s32 Result;
    connection_info *ConnectionInfo = (connection_info *)lpParameter;
	while(true)
    {
        //printf("[thread_proc index=%d id=%d] tick\n", ConnectionInfo->Index, ConnectionInfo->ThreadID);
        char RecvMessage[256] = "";
		Result = recv(ConnectionInfo->Socket, RecvMessage, sizeof(RecvMessage), 0);
        if(Result > 0)
        {
            char SendMessage[256] = "";
            FormatString(sizeof(SendMessage), SendMessage, 
                         "%s:%d: %s",
                         inet_ntoa(ConnectionInfo->Addr.sin_addr), htons(ConnectionInfo->Addr.sin_port), RecvMessage);
            printf(SendMessage);
            for(u32 Index = 0; 
                Index < MAX_CONNECTION_COUNT; 
                Index++)
            {
                /* 
                                if(Index == ConnectionInfo->Index)
                                {
                                    continue;
                                }
                 */
                connection_info *ConnectionInfo = ConnectionInfoArray + Index;
                if(ConnectionInfo->IsInitialized)
                {
                    send(ConnectionInfo->Socket, SendMessage, sizeof(SendMessage), 0);
                }
            }
        }
        else
        {
            int ErrorCode = WSAGetLastError();
            switch(ErrorCode)
            {
                case WSAECONNRESET:
                {
                    char DisconnectMessage[256] = "";
                    FormatString(sizeof(DisconnectMessage), DisconnectMessage,
                                 "client#%d %s:%d disconnected from server\n",
                                 ConnectionInfo->TmpIndex, inet_ntoa(ConnectionInfo->Addr.sin_addr), htons(ConnectionInfo->Addr.sin_port));
                    printf(DisconnectMessage);
                    for(u32 Index = 0; 
                        Index < MAX_CONNECTION_COUNT; 
                        Index++)
                    {
                        if(Index == ConnectionInfo->TmpIndex)
                        {
                            continue;
                        }
                        connection_info *ConnectionInfo = ConnectionInfoArray + Index;
                        if(ConnectionInfo->IsInitialized)
                        {
                            send(ConnectionInfo->Socket, DisconnectMessage, sizeof(DisconnectMessage), 0);
                        }
                    }
                    
                    closesocket(ConnectionInfo->Socket);
                    ConnectionInfo->IsInitialized = false;
                    
                    ExitThread(0);
                } break;
                
                default:
                {
                    LPSTR ErrorString = 0;
                    int Size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                                             0,
                                             ErrorCode,
                                             MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                                             (LPSTR)&ErrorString, 0, 0);
                    printf("[thread_proc index=%d id=%d] Error code %d. Message(%d): %s\n",
                           ConnectionInfo->TmpIndex, ConnectionInfo->ThreadID, ErrorCode, Size, ErrorString);
                    LocalFree(ErrorString);
                } break;
            }
            
            ExitThread(0);
        }
        Sleep(1000);
    }
}

#if APP_NO_CRT
extern "C" int __stdcall mainCRTStartup(void)
#else
int __cdecl main(s32 Argc, char **Argv)
#endif
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
            char ConnectionMessage[256] = "";
            
            // NOTE(ezexff): try find space for new connection in array
            connection_info *FreeConnectionInfoSlot = 0;
            u32 TmpIndex = 0; // TODO(ezexff): mb remove
            for(u32 Index = 0;
                Index < MAX_CONNECTION_COUNT;
                ++Index)
            {
                if(!ConnectionInfoArray[Index].IsInitialized)
                {
                    ConnectionInfoArray[Index].IsInitialized = true;
                    FreeConnectionInfoSlot = &ConnectionInfoArray[Index];
                    TmpIndex = Index;
                    break;
                }
            }
            
            if(FreeConnectionInfoSlot)
            {
                FreeConnectionInfoSlot->TmpIndex = TmpIndex;
                FreeConnectionInfoSlot->Addr = Addr;
                FreeConnectionInfoSlot->Socket = Connection;
                
#if 0
                //~ NOTE(ezexff): printf vs writeconsole bench
                for(u32 i = 0;
                    i < 100;
                    i++)
                {
#if APP_NO_CRT
                    u64 Clock =  __rdtsc();
                    printf("client#%d %s:%d connected to server\n",
                           TmpIndex, inet_ntoa(Addr.sin_addr), htons(Addr.sin_port));
                    Clock = __rdtsc() - Clock;
                    printf("PrintInConsole = %llu\n", Clock);
#else
                    u64 Clock = __rdtsc();
                    printf("client#%d %s:%d connected to server\n",
                           TmpIndex, inet_ntoa(Addr.sin_addr), htons(Addr.sin_port));
                    Clock = __rdtsc() - Clock;
                    printf("printf = %llu\n", Clock);
#endif
                    Sleep(10);
                }
#endif
                
                FormatString(sizeof(ConnectionMessage), ConnectionMessage,
                             "client#%d %s:%d connected to server\n",
                             TmpIndex, inet_ntoa(Addr.sin_addr), htons(Addr.sin_port));
                printf(ConnectionMessage);
                for(u32 Index = 0; 
                    Index < MAX_CONNECTION_COUNT; 
                    Index++)
                {
                    connection_info *ConnectionInfo = ConnectionInfoArray + Index;
                    if(ConnectionInfo->IsInitialized)
                    {
                        send(ConnectionInfo->Socket, ConnectionMessage, sizeof(ConnectionMessage), 0);
                    }
                }
                
                CreateThread(0, 0, ThreadProcConnection, FreeConnectionInfoSlot, 0, (DWORD *)&FreeConnectionInfoSlot->ThreadID);
            }
            else
            {
                FormatString(sizeof(ConnectionMessage), ConnectionMessage, "Server is full!!!");
                send(Connection, ConnectionMessage, sizeof(ConnectionMessage), 0);
                closesocket(Connection);
            }
		}
	}
    
    closesocket(ListenConnection);
    WSACleanup();
    
    ConsolePause();
    ExitProcess(0);
}