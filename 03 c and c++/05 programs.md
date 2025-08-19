# Programs

Ниже приведены примеры программ на C/C++

## Windows program

```C
#include <windows.h>

int WINAPI
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nShowCmd)
{
    OutputDebugStringA("Test Debug Str");
}
```

Print char string
```C
char Buffer[256];
wsprintf(Buffer, "Thread %u: %s\n", GetCurrentThreadId(), (char *)Data);
OutputDebugStringA(Buffer);
```



## Align value

```C
#define Align4(Value) ((Value + 3) & ~3)
#define Align8(Value) ((Value + 7) & ~7)
#define Align16(Value) ((Value + 15) & ~15)
```

Выравнивание однобайтового числа до четырёх
1. Value = 2(10) = 00000010(2)
2. (Value + 3) = 00000010(2) + 00000011(2) = 00000101(2) = 5(10)
3. ~3 = ~00000011(2) = 11111100(2) = -4(10)
4. ((Value + 3) & ~3) = 00000101(2) & 11111100(2) = 00000100(2) = 4(1)


## Endianness check

Алгоритм
* 1. Присваиваем переменной х двухбайтового типа единицу
* 2. Получаем указатель на первый байт переменной в памяти
  * `&x` получаем адрес в памяти двухбайтовой переменной
  * `(unsigned short *)&x` кастим переменную по адресу до одного байта, отсекая второй байт
  * `*((unsigned short *)&x)` получаем доступ к числу, хранящемся в первом байте
* 3. Проверка
  * если у нас в первом байте число 1, то это little-endian
  * если у нас в первом байте число 0, то это big-endian, т.к. единица записана во второй байт

Программа для определения порядка байтов памяти (endianness check)
```C
#include <windows.h>
#include <stdio.h>

int WINAPI
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nShowCmd)
{
    unsigned char x = 1;
    bool Test = *((unsigned short *)&x);
    printf("%s-endian\n", Test ? "little" : "big");
}
```
