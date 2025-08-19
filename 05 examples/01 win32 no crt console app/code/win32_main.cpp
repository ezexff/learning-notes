#define PROGRAM_NUMBER 5

#if PROGRAM_NUMBER == 1

#include <windows.h>

struct teststruct
{
    int Value;
};

void
foo()
{
    OutputDebugStringA("12345");
}

extern "C" int __stdcall WinMainCRTStartup(void)
/*int WINAPI
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nShowCmd)*/
{
    /*OutputDebugStringA("Test Debug Str");
    printf("Test Console Str");
    
    unsigned char Test = 255;
    Test += 1;
    int tmp = 5;
    
    uint16_t x = 0x0001;
    bool Test = *((uint8_t *)&x);
    printf("%s-endian\n", Test ? "little" : "big");
    teststruct Array[50];
	teststruct *ArrayPointer = Array;
	Array[25].Value = 10;
	(ArrayPointer + 25)->Value = 20;
    ((teststruct *)((char *)ArrayPointer + 25 * sizeof(teststruct)))->Value = 30;
    
    unsigned char x = 1;
    bool Test = *((unsigned short *)&x);*/
    
    foo();
    
    
    int Value = 17;
    int *Pointer = 0;
    Pointer = &Value;
    (*Pointer) = 12;
    int V1 = sizeof(Pointer);
    int V2 = sizeof(*Pointer);
    
    int end = 1;
    MessageBox(0, "Test text", 0, MB_YESNO);
    
    ExitProcess(0);
}
#endif

#if PROGRAM_NUMBER == 2
int Five(void)
{
    return(5);
}

typedef int number_function(void);

extern "C" int __stdcall WinMainCRTStartup()
{
    number_function *Function = Five;
    int Number = Function();
    
    return(0);
}
#endif

#if  PROGRAM_NUMBER == 3
unsigned char CodeForFive[] = {0xB8, 0x05, 0x00, 0x00, 0x00, 0xC3};

typedef int number_function(void);

extern "C" int __stdcall WinMainCRTStartup(void)
{
    number_function *Function = (number_function *)(&CodeForFive[0]);
    int Number = Function();
    
    return(0);
}
#endif

#if PROGRAM_NUMBER == 4

#include <windows.h>

extern "C" void __declspec(dllimport) *VirtualAlloc(void *lpAddress,
                                                    SIZE_T dwSize,
                                                    DWORD  flAllocationType,
                                                    DWORD  flProtect);

typedef int number_function(void);

extern "C" int __stdcall WinMainCRTStartup(void)
{
    unsigned char *Code = (unsigned char *)VirtualAlloc(0, 4096, 0x00001000, 0x40);
    // we put 5 and return
    Code[0] = 0xb8;
    Code[1] = 0x05;
    Code[2] = 0x00;
    Code[3] = 0x00;
    Code[4] = 0x00;
    Code[5] = 0xc3;
    
    number_function *Function = (number_function *)(&Code[0]);
    int Number = Function();
    
    return(0);
}
#endif

#if PROGRAM_NUMBER == 5
extern "C" void __declspec(dllimport) *VirtualAlloc(void *lpAddress,
                                                    unsigned __int64 dwSize,
                                                    unsigned int flAllocationType,
                                                    unsigned int flProtect);
int Five(void)
{
    return(5);
}

typedef int number_function(void);

// extern "C" int __stdcall WinMainCRTStartup(void)
extern "C" int __stdcall mainCRTStartup(void)
{
    unsigned char *Code = (unsigned char *)VirtualAlloc(0, 4096, 0x00001000, 0x40);
    // FF 15 5B 1F 00 00 - offset 32 bit call
    
    // FF 15 5B 1F 00 00 C3 - our generated code (call and return), where
    // FF 15 - preambule for call
    // 1F 5B - offset to func address
    // C3 - return
    
    // instead putting 5 we will put call to five function generated code
    
    // Наш собственный код для вызова функции Five
    // 1. Code[0-1] преамбула инструкции call
    // 2. Code[2] 4 байта смещение до адреса функции Five
    // 3. Code[6] возврат из инструкции call
    // 4. Code[7] адрес функции Five, которую вызовет наша инструкция
    Code[0] = 0xff;
    Code[1] = 0x15;
    *(__int64 *)&Code[2] = &Code[7] - &Code[6]; // jump location: diff between our func end and where need read ptr
    Code[6] = 0xc3;
    *(void **)&Code[7] = Five;
    
    number_function *Function = (number_function *)Code;
    int Number = Function();
    
    return(0);
}
#endif

#if PROGRAM_NUMBER == 6

typedef void *HANDLE;
#define NULL 0
#define STD_OUTPUT_HANDLE ((unsigned int)-11)
#define INVALID_HANDLE_VALUE ((HANDLE *)-1)

extern "C" void __declspec(dllimport) *VirtualAlloc(void *lpAddress,
                                                    unsigned __int64 dwSize,
                                                    unsigned int flAllocationType,
                                                    unsigned int flProtect);

extern "C" HANDLE __declspec(dllimport) GetStdHandle(unsigned int nStdHandle);

extern "C" bool __declspec(dllimport) WriteConsoleA(HANDLE hConsoleOutput,
                                                    const void *lpBuffer,
                                                    unsigned int nNumberOfCharsToWrite,
                                                    unsigned long *lpNumberOfCharsWritten,
                                                    void *lpReserved);

extern "C" char _declspec(dllimport) *GetCommandLineA();

extern "C" int __stdcall mainCRTStartup(void)
{
    unsigned char *Memory = (unsigned char *)VirtualAlloc(0, 4096, 0x00001000, 0x40);
    char *Text = GetCommandLineA();
    int Count = 0;
    if (Text && Memory)
    {
        char *TextPtr = Text;
        while(*TextPtr++)
        {
            Memory[Count] = *(TextPtr - 1);
            Count++;
        }
    }
    
    HANDLE OutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    unsigned long CharsWritten = 0;
    if((OutputHandle != NULL) && (OutputHandle != INVALID_HANDLE_VALUE))
    {
        WriteConsoleA(OutputHandle, Memory, Count, &CharsWritten, 0);
    }
    
    char *Text2 = "\nHello World!\n";
    char *Text2Ptr = Text2;
    Count = 0;
    while(*Text2Ptr++)
    {
        Count++;
    }
    if(CharsWritten > 0)
    {
        WriteConsoleA(OutputHandle, Text2, Count, &CharsWritten, 0);
    }
    
    return(0);
}
#endif