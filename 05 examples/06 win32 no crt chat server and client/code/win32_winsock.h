HANDLE OutputStdHandle;
HANDLE InputGetStdHandle;

internal void
InitConsole()
{
    OutputStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    InputGetStdHandle = GetStdHandle(STD_INPUT_HANDLE);
}

#if APP_NO_CRT
internal void
printf(char *Format, ...)
{
    va_list ArgList;
    va_start(ArgList, Format);
    char Buffer[256] = "";
    DWORD Length = (DWORD)FormatStringList(sizeof(Buffer), Buffer, Format, ArgList);
    DWORD Written = 0;
    va_end(ArgList);
    WriteConsole(OutputStdHandle, Buffer, Length, 0, 0);
}
#endif

internal void
PrintLastError()
{
    int ErrorCode = WSAGetLastError();
    LPSTR ErrorString = 0;
    int Size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                             0,
                             ErrorCode,
                             MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                             (LPSTR)&ErrorString, 0, 0);
    printf("\n\n\nError code %d. Message(%d): %s", ErrorCode, Size, ErrorString);
    //printf("\n\n\nError code %d. Message(%d): %s", ErrorCode, Size, ErrorString);
    LocalFree(ErrorString);
}

internal void
ConsolePause()
{
    printf("Enter 'q' to exit\n");
    char Buffer[32];
    b32 IsPause = true;
    while(IsPause)
    {
        DWORD CharsRead;
        ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), Buffer, sizeof(Buffer), &CharsRead, 0);
        if(Buffer[0] == 'q')
        {
            IsPause = false;
        }
    }
}