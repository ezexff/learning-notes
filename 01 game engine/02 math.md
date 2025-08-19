# Math

### Lerp

`(1 - t) * S + t * E`, где  
* t - время [0, 1], в промежутке от 0 до 1
* S - starting value
* E - ending value

### How avoid float precision problem?

* Doing things in very small scales
* [What Every Computer. Scientist Should Know About Floating-Point Arithmetic, by David Goldberg](https://ece.uwaterloo.ca/~dwharder/NumericalAnalysis/02Numerics/Double/paper.pdf)
* Real Computing Made Real: Preventing Errors in Scientific and Engineering Calculations (Dover Books on Computer Science) Illustrated Edition by Forman S. Acton 
* Numerical Methods that Work (Spectrum) First Edition by Forman S. Acton (Author)

### Accumulation vs explicit/inductive calculation

* Explicit/inductive: f(i) -> f(0) + i * delta_f;
* Accumulation: f(i) -> f(0) = ?; f(i) = f(i - 0) + delta_f; where f(i-0) f(previous)
* Accumulation - in this case if we have 400 cycles then we have 400 float round errors

Explicit/inductive (2 float round errors)
```C
float Sum = 100.0f;
float DeltaA = 10.99999f;
float Sum0 = Sum + DeltaA;
for(i = 0; i < 5; i++)
{
    Sum = Sum0 + DeltaA * i;
    ...
}
```

Accumulation (loop count float round errors)
```C
float Sum = 100.0f;
float DeltaA = 10.99999f;
for(i = 0; i < 5; i++)
{
    Sum += DeltaA;
}
```

### Implicit and explicit

* implicit - f(x,y) -> some value, т.е. даём на вход x и y и получаем какое-то значение
* explicit - f(i) -> x,y, т.е. даём на вход какое-то значение и получаем x и y

### Order notation

+ O(n^2) - worst-case running time
+ P (polynomial) vs NP (nondeterministic polynomial)
+ NP-completeness
+ при решении одних проблем можно быть уверенным, что решение всегда займёт строго определённое время, например,
задача сортировки одномерного массива, где худший вариант это O(n^2)
+ при решении других проблем такой уверенности быть не может, например, при решении задачи поиска кратчайшего пути
+ NP-hardness

### Sort

Good sort algorithms
+ Small data (1-60) - Bubble sort
+ Medium data (60-1k) - Radix sort
+ Large data (more than 1k) - Radix sort

Sort stability - with "Stable sort" algorithms we don't change elements order if they already were sorted

### Hash

Hash function
```C
char *Scan = "Test string";
HashValue = 65599 * HashValue + *Scan; // hash(i) = hash(i - 1) * 65599 + str[i]; 
```