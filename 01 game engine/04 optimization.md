# Optimization

Тактовая частота процессора и число циклов
* 3.6 GHz = 3.6 * 1000(GHz) * 1000(MHz) * 1000(KHz) = 3.6 * 1000000000 = 3600000000
* На желаемую частоту кадров 30 FPS доступно циклов = 3.6 GHz / 30 = 120000000
* На желаемую частоту кадров 60 FPS доступно циклов = 3.6 GHz / 60 = 60000000
* На желаемую частоту кадров 144 FPS доступно циклов = 3.6 GHz / 144 = 25000000
* Не все циклы доступны, т.к. работают операционная система и другие программы (на фоне)
* Исходя из числа пикселей на экране имеющееся число циклов достаточно небольшое: 1920*1080 = 2073600

CPU and GPU instructions

SIMD - single instruction, multiple data
* SSE 4-wide
* AVX 8-wide
* AVX512 16-wide
* "SOA" vs "AOS": structure of arrays (want for SIMD) vs array of structers (C language)
* Strided load on NEON
* [Intrinsics Guide](https://www.laruence.com/sse/)
* [Встроенные компоненты x86](https://learn.microsoft.com/ru-ru/cpp/intrinsics/x86-intrinsics-list?view=msvc-170)
* x64 amd64 intrinsics list https://learn.microsoft.com/ru-ru/cpp/intrinsics/x64-amd64-intrinsics-list?view=msvc-170
* Пример встроенного компонента `_mm_sqrt_ss`

Example
```C
__m128 ValueA = _mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f);
__m128 ValueB = _mm_set_ps(10.0f, 100.0f, 1000.0f, 10000.0f);
__m128 Sum = _mm_add_ps(ValueA, ValueB);
```

SVML intrinsics 
```C
_mm_exp_ps
_mm_sin_ps
_mm_cos_ps
_mm_tan_ps
_mm_asin_ps
```



Instructions <-> Cache (L1, L2, L3) <-> Memory (righter=slower)

Cycles
* 3.2 GHz = 3.2 * 1000 GHz * 1000 MHz * 1000 KHz = cycles per second
* 1 core 1 CPU: 3 200 000 000 / 60 = 53 333 333,3 cycles per frame (at 60 fps)
* you won't always have all cycles available for use (OS and other stuff we can't control)
* 1 instruction can be multiple microcodes
* instruction pipelining is a technique for implementing instruction-level parallelism within a single processor

Bandwidth and Throughput Gh/s (how much memory can travel into caches) - это число порций трансфера данных в CPU из RAM. Иными словами это число порций данных, которые трансферятся в CPU из RAM и это число реально увеличить, что позволит повысить производительность

Memory Latency (talks about how long it takes from we ask a piece of memory to when we will that piece of memory back) - это задержка трансфера данных в CPU из RAM. Она зависит от скорости света, т.е. от расстояния от CPU и до RAM (физический предел), следовательно её уменьшить невозможно. For example: latency - 300 cycles when 1 instruction - 2 cycles

Cache Latency for example 16 cycles

Cache miss

Modern optimiziation = how caches fills

Hyperthreading (CPU logically but not physically in 2 states: 1st state in memory latency, 2nd state can do work)

Performance = structuring data and opetations on that data

Efficiency = algorithm

SSE optimization = translation (doing 4 same operations at one time)

[IACA](https://progforperf.github.io/IACA-Guide.pdf) - Intel® Architecture Code Analyzer

[Steam-Hardware-Software-Survey](https://store.steampowered.com/hwsurvey/Steam-Hardware-Software-Survey-Welcome-to-Steam)

cashline 8 pointers = 512 байт для x64 win https://en.algorithmica.org/hpc/cpu-cache/cache-lines/

bandwidth (пропускная способность) CPU намного ниже GPU и поэтому вторая эффективнее

Иногда можно использовать рекурсию, т.к. это легко реализовать, но циклы во много раз быстрее

Как лучше передавать параметры в функцию?
* По значению в функцию эффективно передавать небольшие структуры или переменные (это лучше для компиляторных оптимизаций)
* По указателю в функцию эффективно передавать большие структуры, например, с массивами, т.к. не создаётся копия в памяти этой большой структуры. Так же это позволяет обойтись без возвращаемого значения (можно работать с той же памятью, а не с её копией) и изменять значения переменных структуры по указателю