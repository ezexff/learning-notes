# Computer architecture

Float number representaion
```
| sign | exp | mantissa |
|  1   |  n  |    m     |
```

HEX представление числа
```
0xA(HEX) = 10(DEC)
0xAA(HEX) = 16*10+10(DEC) = 170(DEC)
0xAAA(HEX) = 16*16*10 + 16*10 + 10 = 2730(DEC)
```

Two's complement (дополнительный код)
```
4 bit signed value: 1st bit for sign and 3 bits for value
1111(2)=-7(10)
0111(2)=7(10)=2^3-1
1000(2)=-8(10)
```

Пример преобразования однобайтового отрицательного числа -5, записанного в прямом коде, в дополнительный код
```
Прямой код отрицательного числа -5:
10000101
Инвертируем все разряды числа, кроме знакового, получая таким образом обратный код (первое дополнение) отрицательного числа -5:  
11111010
Добавим к результату 1, получая таким образом дополнительный код (второе дополнение) отрицательного числа -5:  
11111011
```

Endianness - это формат данных в памяти. Переход к старшим разрядам числа по байтам идёт слева направо либо справа налево
* little-endian - x86, arm, x64 (порядок от младшего к старшему)
* big-endian - powerpc (порядок от старшего к младшему)

Например, в памяти little endian число 500 с типом данных 2 байта unsigned short будет выглядеть следующим образом:
```
2 байта DEC в памяти: 244 1
Первый байт это младшие разряды, а второй байт старшие разряды, т.е. 2ой байт 1(DEC) в памяти это 255+1=256
2 байта BIN в памяти: 11110100 0000001
Тогда как в бинарном виде число 500, состоящее из двух байтов, выглядит следующим образом:
       5 1
       1 2631
       2 84268421
00000001 11110100
```


## Where code lives?

* Our code lives in .exe file
* Code -> compiler -> obj -> linker (libs) -> .exe (microsoft/intel format)
* .exe это header + fixed table + intel code
* OS transfer intel code to memory

Virtual memory is a system where OS wake up when smb need a page not there and does smth

VMAS (virtual memory address space)
* VMAS starts from 0 and can be high number (for example, 4 billion)
* VMAS contains pages (usually size of page 4096 or 64k bytes)
* A page table is the data structure used by a virtual memory system in a computer operating system to store the mapping between virtual addresses and physical addresses. Virtual addresses are used by the program executed by the accessing process, while physical addresses are used by the hardware, or more specifically, by the random-access memory (RAM) subsystem. The page table is a key component of virtual address translation that is necessary to access data in memory.
* Page has flags: read, write, exec (execute code)
* OS unset flags when your data from page moved by OS on hardware/disk
* Page fault (when you trying access to old page with unset flags)

If your program has bug and you trying write to unallocated memory this causes Access violations

Function calls is stack of pages in VMAS

Stack (not heap or smth else) uses for function calling because our new functions in VMAS needs old functions who called these new functions)

HDR - high dynamic range помогает использовать цвета большого диапазона, т.к. обычно в GPU цвет это (u8 r,g,b,a) с крайне ограниченным диапазоном [0..255], а не (r32 r,g,b,a)

Крайне нежелательно использовать нагромождение скобок и указателей массивов. Лучше всегда использовать typedef, чтобы не возникало путаницы у того кто читает код
```C
u8 Array[3][2] = {}; // это массив, который состоит из трёх массивов по 2 элемента u8
u8 *Foo[3][2] = {}; // *Foo это массив трёх массивов состоящих из двух указателей к u8 *
```
