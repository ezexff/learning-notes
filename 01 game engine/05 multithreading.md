# Multithreading

Multithreading brief
* Threads (processes in OS creates threads for multithreading)
* Physical cores (number of hardware components)
* Logical cores (number of threads that can run on each physical core through use of hyperthreading)

Сreating a thread (as allocating memory) is very expensive at OS level (for game we need init this on app start)

WINAPI Thread creation
```C
DWORD WINAPI //
ThreadProc(LPVOID lpParameter)
{
    char *StringToPrint = (char *)lpParameter;
    for(;;)
    {
        Log.AddLog(StringToPrint);
        Sleep(1000);
    }
}
char *Param = "Thread started!\n";
DWORD ThreadID;
HANDLE ThreadHandle = CreateThread(0, 0, ThreadProc, Param, 0, &ThreadID);
CloseHandle(ThreadHandle);
```

WINAPI Thread synchronization
* Ключевое слово volatile информирует компилятор, что значение переменной может меняться извне. Переменная не оптимизируется
* Compiler and processor fences - барьеры, которые заставляют выполнять код в нужном порядке, без оптимизаций
* Ограничение оптимизаций компилятора, которые могут изменить порядок доступа к памяти для точки вызова
  * _ReadWriteBarrier(); _mm_fence();
  * _WriteBarrier(); _mm_sfence();
  * _ReadBarrier(); _mm_lfence();
* Interlocked operations или функции, которые создают полный барьер памяти (или ограждение), чтобы гарантировать, что операции с памятью выполняются по порядку. Например:
  * InterlockedCompareExchange();
  * InterlockedIncrement();
* Semaphore позволяет ограничить доступ потоков к объекту синхронизации на основании их количества. Например:
  * CreateSemaphore(); создание семафора
  * ReleaseSemaphore() увеличивает значение счетчика
*  WaitForSingleObjectEx(); блокирует выполнение потока до наступления какого-то события или тайм-аута (уменьшает значение семафора)

Example TaskWithMemory() API
```C
SubArena();
BeginTemporaryMemory();
EndTemporaryMemory();
BeginTaskWithMemory();
EndTaskWithMemory();
```

## WorkQueue
Структура очереди работ
```C
struct platform_work_queue
{
    u32 volatile CompletionGoal;
    u32 volatile CompletionCount;
    
    u32 volatile NextEntryToWrite; // индекс добавленной работы
    u32 volatile NextEntryToRead; // индекс работы, которую надо выполнить
    HANDLE SemaphoreHandle;
    
    platform_work_queue_entry Entries[256];
};
```

Создание очереди работ
- platform_work_queue HighPriorityQueue = {};
- Вызываем метод создания очереди Win32MakeQueue(&HighPriorityQueue, 6);
- Создаём семафор, где начальное состояние 0 и число обращений равно числу тредов, т.е. 6
- Создаём треды, ожидающие добавление работы (6 шт.)

Ожидание добавления новых работ в тредах - метод ThreadProc()
- В методе Win32DoNextWorkQueueEntry() сравниваем переменные Queue->NextEntryToRead и Queue->NextEntryToWrite, которые показывают сколько работ в очереди и сколько работ выполняется
- Если переменные не равны, то начинаем выполнение следующей в очереди работы
- Если переменные равны, то нет новых работ - методом WaitForSingleObjectEx() переводим тред в неактивный режим

Добавление работы
- Определяем индекс новой работы
- Присваиваем работе указатели на функцию, которую надо выполнить и на структуру, передаваемую в эту функцию
- Присваиваем Queue->NextEntryToWrite индекс новой работы
- Увеличиваем счётчик семафора на 1

## InterlockedCompareExchange
```C
LONG InterlockedCompareExchange(
  [in, out] LONG volatile *Destination,
  [in]      LONG          ExChange,
  [in]      LONG          Comperand
);
```
Функция сравнивает текущее значение переменной типа LONG (на которую указывает параметр Destination) со значением, передаваемым в параметре Comparand. Если значения совпадают, *Destination получает значение параметра ExChange; в ином случае *Destination остается без изменений. Функция возвращает исходное значение *Destination. Все эти действия выполняются как единая атомарная операция