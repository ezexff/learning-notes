# Reverse engineering

Software for reverse engineering
* x64dbg
* cutter
* ida
* ghidra

В отладчике дизассемблера Visual Studio байты правее адреса и до названия инструкции (48 8D 44 24 50) это зашифрованная инструкция процессора
```
000000000000002D: 48 8D 44 24 50     lea         rax,[rsp+50h]
```

У операндов asm инструкций сначала идёт вывод, а потом ввод. 1ый операнд это куда запишем результат, а второй операнд это откуда берём значение для вычислений

Регистры процессора
* RIP хранит адрес исполняемой инструкции. Во время дебаггина в VS Studio мы можем изменить значение регистра на произвольное, например, чтобы пропустить строку кода с возникшим исключением. Иными словами, на процессоре имеется специальный регистр, который содержит адрес виртуальной памяти ссылающийся на инструкцию исполняемую в данный момент. После выполнения инструкции этот регистр увеличивается на число байт выполненной инструкции и переходит к выполнению следующей
* RSP это указатель на стек программы в виртуальной памяти

CMP сравнение чисел в ASM это вычитание (в зависимости от знака результата, делаем вывод о сравнении)

Мы можем использовать, вместо кода функции на языке C, последовательность байт, полученную от утилиты dumpbin, но это будет у нас вызывать ошибку, т.к. страница выделенная под виртуальную память переменной CodeForFive без флага executable. Этот флаг необходим для защиты от хакеров
```C
unsigned char CodeForFive[] = {0xB8, 0x05, 0x00, 0x00, 0x00, 0xC3};
typedef int number_function(void);
extern "C" int __stdcall WinMainCRTStartup()
{
    number_function *Function = (number_function *)(&CodeForFive[0]);
    int Number = Function();
}
```

Для того чтобы обойти эту ошибку мы можем заранее выделить память через VirtualAlloc с флагом executable и поместить байты функции Five в страницу, на которой дозволено использовать исполняемый код. Это может быть полезно, если у нас нет кода функции на языке C, но есть .exe. Из этого файла мы извлекаем байты необходимого нам метода и в своей программе, зная сигнатуру, используем функцию

```C
extern "C" void __declspec(dllimport) *VirtualAlloc(
    void *lpAddress,
    SIZE_T dwSize,
    DWORD  flAllocationType,
    DWORD  flProtect);

int Five(void)
{
    return(5);
}

typedef int number_function(void);

extern "C" int __stdcall WinMainCRTStartup(void)
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
```

Find function in .dll by address
* If you don't like to load the NT symbols, there is another method Get the relative address of the function in which you want to set breakpoint with some PE tools.
* For example, just type "dumpbin /exports C:\Windows\System32\user32.dll" in the Visual Studio command line, you can get "RVA" of each exported symbol in "user32.dll". 
* For instance, "RVA" of "GetMessageW" is "000091C6".
* Now check the image base of "user32.dll" in the VS debugger's "Modules" windows, the values is "7E410000" on my laptop (Generally, the system dlls would not be relocated, so the image base value here is equal to the value written in PE file). Then the starting address of "GetMessageW" is "7E410000+000091C6= 7E4191C6". Just set a function breakpoint at this address. Then the debugger will stop when calling into "GetMessageW"

Пути стандартных Windows библиотек
* х64 C:\Windows\SysWOW64
* x32 C:\Windows\System32

Relative address in .dll = function address - library address

## Dumpbin

Утилита dumpbin для исследования сгенерированного объектного/исполняемого файла. Пример использования
```
dumpbin /disasm win32_main.obj
```

Сохранить дизассемблированные .obj файлы и .exe файл
```
dumpbin /disasm main.exe > main_exe.asm
dumpbin /disasm main.obj > main_obj.asm
dumpbin /disasm main3.obj > main3_obj.asm
```

Через утилиту dumpbin мы можем посмотреть какие зависимости у .exe файла
```
dumpbin /imports win32_main.exe
```

Через утилиту dumpbin мы можем посмотреть какие методы .dll файла может вызывать Windows Dynamic linker
```
dumpbin /exports win32_main.exe
```

Через утилиту dumpbin мы можем узнать как хранятся части данных нашей программы, где RAW DATA #1 это сама программа, а RAW DATA #2 вызов VirtualAlloc из kernel32.dll. Понять что RAW DATA #2 это вызов Virtual Alloc можно сопоставив адрес из опции /imports с адресом из опции /rawdata (этот адрес рекомендуемый для запускаемой программы, Windows Dynamic linker при каждом запуске программы генерирует случайный адрес, для безопасности)
```
dumpbin /rawdata win32_main.exe
```

 Compare 2 exe files in dumpbin
```
dumpbin /all /rawdata:none win32_main.exe > file_with_gl_and_ltcg.txt
dumpbin /all /rawdata:none win32_main.exe > file_without_gl_and_ltcg.txt
```

Create files with imports/exports of .exe/.dll
```
dumpbin /exports win32_main.exe > exports.txt
dumpbin /imports win32_main.exe > imports.txt
dumpbin /exports C:\Windows\System32\gdi32full.dll > gdi32full_exports.txt
dumpbin /exports C:\Windows\SysWOW64\gdi32full.dll > gdi32full_exports.txt
```

https://en.wikipedia.org/wiki/X86_instruction_listings
https://en.wikipedia.org/wiki/Time_Stamp_Counter

WIN API
+ user32.lib - create window
+ gdi32.lib - graphics functions like GetDeviceCaps(...)
+ winmm.lib - timer resolution for windows scheduler
+ 
OpenGL
+ opengl32.lib - opengl