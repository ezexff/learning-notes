#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#if (!APP_NO_CRT)
#include <stdio.h>
#endif

#include <windows.h>
#include <winsock2.h>
#include "engine_base.h"
#include "win32_winsock.h"

SOCKET Connection;
b32 ConnectionIsActive = false;

DWORD WINAPI
ThreadProcConnection(LPVOID lpParameter)
{
    // NOTE(ezexff): recv message
	while(ConnectionIsActive)
    {
        char Message[256] = "";
        //printf("[thread] tick\n");
        s32 Result = 0;
        Result = recv(Connection, Message, sizeof(Message), 0);
        if(Result > 0)
        {
            printf(Message);
        }
        else
        {
            ConnectionIsActive = false;
            int ErrorCode = WSAGetLastError();
            switch(ErrorCode)
            {
                case WSAECONNRESET:
                {
                    printf("[thread] Lost connection to server\n");
                } break;
                
                default:
                {
                    PrintLastError();
                } break;
            }
            return(1);
        }
        
        Sleep(1000);
    }
    return(0);
}

#if APP_NO_CRT
extern "C" int __stdcall mainCRTStartup(void)
#else
int __cdecl main(s32 Argc, char **Argv)
#endif
{
    s32 Result = 0;
    
    InitConsole();
    
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
    
    SOCKADDR_IN Addr;
    int sizeofaddr = sizeof(Addr);
    Addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    Addr.sin_port = htons(1111);
    Addr.sin_family = AF_INET;
    
    Connection = socket(AF_INET, SOCK_STREAM, 0);
    if(Connection == INVALID_SOCKET)
    {
        PrintLastError();
        WSACleanup();
        ConsolePause();
        ExitProcess(0);
    }
    
    Result = connect(Connection, (SOCKADDR*)&Addr, sizeof(Addr));
    if(Result == SOCKET_ERROR)
    {
        PrintLastError();
        WSACleanup();
        ConsolePause();
        ExitProcess(0);
    }
    
    // NOTE(ezexff): send message
    CreateThread(0, 0, ThreadProcConnection, 0, 0, 0);
    ConnectionIsActive = true;
    while(ConnectionIsActive)
    {
        char Message[256] = "";
        DWORD CharsRead;
        ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), Message, sizeof(Message), &CharsRead, 0);
        //fgets(Message, sizeof(Message), stdin);
        Result = send(Connection, Message, sizeof(Message), 0);
        if(Result == SOCKET_ERROR)
        {
            PrintLastError();
            WSACleanup();
            ConsolePause();
            ExitProcess(0);
        }
        Sleep(1000);
    }
    
    closesocket(Connection);
    WSACleanup();
    
    ConsolePause();
    ExitProcess(0);
}