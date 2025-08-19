/*
Guide - How to avoid C/C++ runtime on Windows by mmozeiko (Mārtiņš Možeiko)

Now you can write pretty much almost any reasonable code as you wish!

Remember that you are not allowed to use following features:

 1. C++ RTTI (it's turned off by -GR- anyway)
2. C++ exceptions - try/catch/throw (this it's turned off by -EHa-)
3. SEH exceptions - you could use them if you implement _C_specific_handler (for 64-bit code) and _except_handler3 (for 32-bit code) functions. See simple expample how to do that by calling original C runtime functions in win32_crt_seh.cpp file.
4. Global objects with C++ constructors and destructors - it's possible to implement it, but it's not needed for us.
5. Pure virtual functions in C++ classes - for that you'll need to implement "__purecall" function, but we are also not interested in this.
6. No new/delete C++ operators, they are using global new/delete functions. You'll need to either override new/delete for each class, or implement global new/delete functions yourself.

Of course you can not use any function from standard C or C++ runtime (stdlib.h, stdio.h, string.h, math.h, etc.. headers). Only safe headers are the ones that provide compiler specific functionality, such as: stddef.h - if you want size_t and NULL stdint.h - various intXX_t and uintXX_t typedefs stdarg.h - va_arg, va_start, va_end, va_arg intrinsics intrin.h and few other headers for intrinsic functions (rdtsc, cpuid, SSE, SSE2, etc..)
*/

// NOTE(ezexff): Floating point fix
extern "C"
{
    int _fltused;
}

#define DllImport __declspec(dllimport)

// NOTE(ezexff): Kernel32 imports
extern "C" void DllImport *GetModuleHandleA(char *lpModuleName);

extern "C" void DllImport ExitProcess(unsigned int uExitCode);

extern "C" void DllImport *VirtualAlloc(void *lpAddress,
                                        unsigned __int64 dwSize,
                                        unsigned int flAllocationType,
                                        unsigned int flProtect);

extern "C" bool DllImport VirtualFree(void *lpAddress,
                                      unsigned __int64 dwSize,
                                      unsigned int dwFreeType);

extern "C" void DllImport *LoadLibraryA(char *lpLibFileName);

extern "C" void DllImport *GetProcAddress(void *hModule,
                                          char *lpProcName);

// NOTE(ezexff): User32.dll imports
extern "C" int DllImport MessageBoxA(void *hWnd,
                                     char *lpText,
                                     char *lpCaption,
                                     unsigned int uType);


// NOTE(ezexff): win32_library.dll imports
#define SQUARE_TEST_FUNCTION(name) int name(int Value)
typedef SQUARE_TEST_FUNCTION(square_test_function);
square_test_function *SquareTestFunction;

#define INT32_DIGITS_COUNT(name) int name(int Value)
typedef INT32_DIGITS_COUNT(int32_digits_count);
int32_digits_count *Int32DigitsCount;

#define INT32_TO_CHARSTR(name) unsigned char * name(unsigned char *Memory, int Value, unsigned int ValueDigitsNumber)
typedef INT32_TO_CHARSTR(int32_to_charstr);
int32_to_charstr *Int32ToCharStr;

// NOTE(ezexff): VirtualAlloc constants
#define MEM_COMMIT 0x00001000
#define PAGE_READWRITE 0x04
#define MEM_RELEASE 0x00008000
//#define PAGE_EXECUTE_READWRITE 0x40

// NOTE(ezexff): MessageBox constants
#define MB_OK 0x00000000L
#define MB_YESNO 0x00000004L
#define MB_ICONQUESTION 0x00000020L
//#define MB_ICONWARNING 0x00000030L
#define IDYES 6
#include <stdint.h>

extern "C" void __stdcall WinMainCRTStartup(void)
{
    // NOTE(ezexff): Without CRT tests
    // 1. Allocating large arrays/structure on stack (>4KB)
    {
        char BigArray[4096];
        BigArray[0] = 0;
        // Fixed with compiler and linker options
        // cl: /GS- /Gs9999999
        // link: /STACK:0x100000,0x100000
    }
    
    // 2. Some calculations with 64-bit integers in 32-bit code
    {
        double t456 = 5.0f;
        double t457 = 100000.0f;
        t456 = t456 * t457;
        // We can skip for x64 build
        // x64 build and CPU Ryzen 5 3600 - these lines of code work in xmm0 register
    }
    
    // 3. Using floating point
    {
        float t123 = 55.0f;
        // Fixed with _fltused;
        /*
    extern "C"
    {
        int _fltused;
    }
        */
    }
    
    // 4. Casting floating point to integer and back in 32-bit code
    {
        float f = 1000.0f;
        double d = 1000000000.0;
        
        int32_t i32f = (int32_t)f;
        int32_t i32d = (int32_t)d;
        uint32_t u32f = (uint32_t)f;
        uint32_t u32d = (uint32_t)d;
        
        int64_t i64f = (int64_t)f;
        int64_t i64d = (int64_t)d;
        uint64_t u64f = (uint64_t)f;
        uint64_t u64d = (uint64_t)d;
        
        f = (float)i32f;
        d = (double)i32d;
        f = (float)u32f;
        d = (double)u32d;
        
        f = (float)i64f;
        d = (double)i64d;
        f = (float)u64f;
        d = (double)u64d;
        // We can skip for x64 build
    }
    
    // 5. initialization and assignment of large arrays/structures
    {
        char BigArray2[100] = {};
    }
    
    // NOTE(ezexff): Run-Time Dynamic Linking
    void *TestLibrary = LoadLibraryA("win32_library.dll");
    if(TestLibrary)
    {
        // Import win32_library.dll functions
        SquareTestFunction = (square_test_function *)GetProcAddress(TestLibrary, "Square");
        Int32DigitsCount = (int32_digits_count *)GetProcAddress(TestLibrary, "Int32DigitsCount");
        Int32ToCharStr = (int32_to_charstr *)GetProcAddress(TestLibrary, "Int32ToCharStr");
        
        if(SquareTestFunction &&
           Int32DigitsCount &&
           Int32ToCharStr)
        {
            void *Instance = GetModuleHandleA(0);
            
            int Count = 0;
            while (IDYES != (MessageBoxA(0, "Close program?", "No button counter", MB_YESNO | MB_ICONQUESTION)))
            {
                Count++;
            }
            
            //int Value = 1234;
            int Value = 0;
            
            if(Count > 0)
            {
                Count = SquareTestFunction(Count);
                Value += Count;
            }
            
            int ValueDigitsNumber = Int32DigitsCount(Value);
            
            unsigned char *Memory = (unsigned char *)VirtualAlloc(0, ValueDigitsNumber, MEM_COMMIT, PAGE_READWRITE);
            
            unsigned char *StrValue = Int32ToCharStr(Memory, Value, ValueDigitsNumber);
            
            MessageBoxA(0, (char *)StrValue, "1234 + Square(NoButtonCounter)", MB_OK);
            
            VirtualFree(Memory, (unsigned int)ValueDigitsNumber, MEM_RELEASE);
        }
        else
        {
            MessageBoxA(0, "Can't import function from win32_library.dll", 0, MB_OK);
        }
    }
    else
    {
        MessageBoxA(0, "Unable to load win32_library.dll", 0, MB_OK);
    }
    
    ExitProcess(0);
}