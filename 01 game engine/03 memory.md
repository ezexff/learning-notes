# Memory

Pre-allocated big chunk of memory with VirtualAlloc()
* Permanent memory (initialized once and doesn't change)
* Transient memory (clear and fill at every frame)
<p align="center"><img src="https://i.imgur.com/L2ws8Fz.png" width="150"></p>

Размер виртуальной памяти
* If our all assets less than 4GB and we have 4GB RAM then we are done with our memory alloc system
* If our all assets more than 4GB (for example 3TB) and we have 4GB RAM then it will cause VM (virtual memory) problem

Example memory allocation on Windows
```C
LPVOID BaseAddress = 0; // for debug LPVOID BaseAddress = (LPVOID)Terabytes(2);
u64 PermanentStorageSize = Megabytes(32);
u64 TransientStorageSize = Megabytes(16);
u64 TotalSize = PermanentStorageSize + TransientStorageSize;
void *GameMemoryBlock = VirtualAlloc(BaseAddress, (size_t)TotalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
void *PermanentStorage = GameMemoryBlock;
void *TransientStorage = ((uint8 *)PermanentStorage + PermanentStorageSize);
```

Example Game Memory API
```C
InitializeArena(MemoryArena, MemoryStorageSize, MemoryStoragePointer);
example_struct *ExampleStruct = PushStruct(MemoryArena, example_struct);
example_struct *ExampleArray = PushArray(MemoryArena, ArraySize, example_struct);

temporary_memory GroundMemory = BeginTemporaryMemory(&TranState->TranArena);
// do some work in temprorary memory
EndTemporaryMemory(GroundMemory);
```

## General Purpose Allocator

Asset Memory Management questions
* How much quantity! Footprint
* "When" Alloc/Free

Fragmentation - проблема заполнения дыр в виртуальной памяти, когда используем heap. Данная проблема особо актуальна, когда пытаешься использовать как можно больше доступной памяти

В мире где все ассеты одного размера, то и нет проблемы фрагментации. Один ассет уходит, а другой заполняет его место

"Fixed size allocators" - хороши, т.к. отличное управление памяти, нет фрагментации, это просто реализовать

В случае, когда ассеты разного размера, например, изображения, то мы можем их приводить к одному размеру, например 256х256

Если мы не хотим терять в качестве, то можем разбивать большие изображения на тайлы, например, изображение 512х512 можно разбить на 4 тайла 256х256

Два варианта реализации аллокатора
* Megatexture - огромная текстура, которая содержит все спрайты. Когда нужен определённый спрайт, то загружаем в память чанки/тайлы из которых этот спрайт состоит
    * При подобной реализации проблемным становится Mipmapping
    * Всё ещё остаётся проблема, когда 1 ассет 512x512 - 4 тайла (256х256), а другой ассет 256х256 лишь 1 тайла, т.е. в памяти 1ый случай это 4 индекса, а второй лишь 1, но это не так существенно, т.к. хранение 4 индексов это очень мало, по сравнению с хранением 256x256x4
* A "variable" allocator (variable sizes)
    * Our memory 4gb
    * Выделяем чанки для трёх ассетов A, B, C
    * Освобождаем память от B и добавляем на это место ассет D (меньшего размера)
    * Пространство между D и C будет пустым и чтобы его заполнить надо производить дефрагментацию
    * Например, когда особождаем память C, то следующий чанк будет размером (B - D) + C

Мегатекстура применима и к звуку - одна большая звуковая дорожка, разбитая на чанки, которая содержит все звуки. 1 чанк будет размером 256x256 байт и целый звук на 4 тайла 256x256x4

Если у нас кап в 3 мегабайта и не хватает памяти для загрузки битмапа, то мы можем между кадрами освободить память битмапа двумя способами
* отсрочить загрузку битмапа до конца кадра
* оставлять некоторое число в памяти для возможной загрузки битмапов

Если у нас кап в 3 мегабайта и не хватает памяти для загрузки битмапа, то мы можем между кадрами освободить память битмапа двумя способами
* отсрочить загрузку битмапа до конца кадра
* оставлять некоторое число в памяти для возможной загрузки битмапов

В конце фрейма выгружать из памяти наименее используемые ассеты, чтобы была возможность загружать новые

Doubly linked list, элемент которой указывает на next и prev. Полезен, когда нужно удалять или добавлять элементы
* при добавлении элемента в между другими элементами листа нужно не только присвоить указатели новому элементу, но ещё и исправить указатели у элементов соседей - суммарно 4 указателя (prev+next нового элемента, next у предыдущего, prev и следующего)
* Sentinel 1st element, у которого указатели prev и next указывают на себя же - с этим элементом лист будет работать как надо, а без него придётся добавлять дополнительные проверки на null
* с наличием Sentinel последний элемент указывает на Sentinel, соответственно лист цикличен
* Non-sentinel имеет дополнительные указатели first и last
* теоретически doubly linked list в 6 раз хуже обычного linked list, т.к. требует 6 записей (по 2 указателя для текущего, прошлого и следующего элементов) 

Recency list
* каждый раз когда мы используем ассет, то перемещаем его в начало листа
* ассеты которые мы реже всего используем окажутся в конце листа и от них мы избавляемся
* sentinel - пустой элемент, который не меняется и относительно которого смотрим MRU и LRU
* next - most recently used (MRU) указатель переходит от начала листа и по часовой стрелке к концу
* prev - least recently used (LRU) указатель переходит от конца листа и против часовой стрелки к началу

Overview of our approach
* Win 64 bit - we can alloc big empty chunk of memory
* Win 32 bit - we need alloc small empty chunks of memory
* We won't be performing small allocations
* Every block will keep information of neighboring blocks

Можно создать гистограмму того какого размера у нас ассеты чтобы понять какой размер превалирует и использовать это число для задания размера блок аллокатора

Загрузка ассета в память
* Ищем в doubly linked list свободный блок памяти и когда нашли, то проверяем
хватит ли места в найденном блоке для ассета и если не хватает - добавляем новый
блок
* Если не удалось найти свободный блок памяти, то избавляемся от
другого блока (от наимее используемого) и используем его

Potential problem of this general-purpose allocator: The linked list could be too
long to walk on each allocation (4000 assets = 8000 links)

