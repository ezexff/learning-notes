# Debug

Визуализация в системе дебаггинга невероятно важна, т.к. позволяет обнаружить баги, которые не проявляются обычным путём

Система дебагинга позволяет увидеть как работает код

Purpose of debug services
1) Coaxing bugs to the surface
2) Locating bugs that are clearly present but difficult to pinpoint

Necessary components: counter log, replay system, log of memory consumption and diagramming

Preprocessor directives (часто используются в дебаг коде)
* `__FILE__`
* `__FUNCTION__`
* `__LINE__`
* `__COUNTER__`

Первый пример
* Для подсчёта числа тактов затраченных функцией или частью текста программы внутри фигурных скобок {} можно использовать структуру с конструктором и деструктором
* В памяти сохраняется информация о последних, например, 128 кадрах
* Для последних 128 кадров рисуется график, на котором наглядно будет видно спайки

Второй пример
- выделяем большой чанк памяти для записи дебаг ивентов (256 последних фреймов)
- флейм граф для общей картины
- вертиальные бары с перформансом 256 фреймов
- горизонтальные бары для 1го фрейма
- табы (вкладки) - threads, frames, clocks

для дебага полезно указатель конвертировать в u32 переменную, которая будет использоваться как цвет

## ImGui
ImGui with from win32 app to .dll needs SetCurrentContext() + SetAllocatorFunctions()

## Performance counters
RDTSC (__rdtsc) and QPC (QueryPerformanceCounter)

Example
```C
typedef struct debug_cycle_counter
{    
    uint64 CycleCount;
    uint32 HitCount;
} debug_cycle_counter;
#define BEGIN_TIMED_BLOCK(ID) uint64 StartCycleCount##ID = __rdtsc();
#define END_TIMED_BLOCK(ID) DebugGlobalMemory->Counters[DebugCycleCounter_##ID].CycleCount += __rdtsc() - StartCycleCount##ID; ++DebugGlobalMemory->Counters[DebugCycleCounter_##ID].HitCount;
#define END_TIMED_BLOCK_COUNTED(ID, Count) DebugGlobalMemory->Counters[DebugCycleCounter_##ID].CycleCount += __rdtsc() - StartCycleCount##ID; DebugGlobalMemory->Counters[DebugCycleCounter_##ID].HitCount += (Count);
#endif
```

## Fonts

Если добавить в С перед строкой, например `L"hello world!"`, то на каждый символ будет выделено по 2 байта, что соответствует `wchar_t` (тип размером 16 bit)

unicode - поддерживает все доступные языки, но слишком большая таблица для игрового движка (подавляющее большинство символов использоваться не будут)

избавиться от лишних символов unicode -> mapping -> new font table (500 glyphs)

unicode has codepoints

Basic typography (metrics): baseline, maximum ascenders and descenders, line height

Kerning (таблица с расстояниями между символами)

Шрифты бывают двух видов
- Monospace
- Proportional

Hinting - very small font (рисуем не по центру двух пикселей, а по смещению в пиксель, чтобы не было проблем с цветом)

3 подхода для реализации в движке
1. Загрузка TTF и тасселяция - разбиваем глиф на треугольники и рисуем треугольники
2. Загрузка TTF и внутренняя растеризация (implicit rasterisation)
3. Использование prerasterized fonts (несколько битмапов со шрифтом для каждого разрешения экрана)

ascent is the coordinate above the baseline the font extends; descent is the coordinate below the baseline the font extends (i.e. it is typically negative) lineGap is the spacing between one row's descent and the next row's ascent... so you should advance the vertical position by "*ascent - *descent + *lineGap" these are expressed in unscaled coordinates, so you must multiply by the scale factor for a given size

Например, шрифт размером 12 пунктов примерно соответствует размеру 16 пикселей или 100% в настройках увеличения шрифта типичного браузера. Типичное соответствие между пт и пикселями составляет около 1 пт = 1.33 пикселей. Таким образом, шрифт размером 12 пунктов будет примерно равен 16 пикселям

