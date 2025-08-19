# Pointers

Указатели и ссылки
* В языке C вы можете при помощи указателя вы можете получить адрес переменной. Адрес показывает где переменная находится в памяти
* Размер указателя зависит от разрядности программы
  * 32-битная версия - 4 байта
  * 64-битная версия - 8 байт

Pointer to a variable
```C
int Value = 17;
int *Pointer = 0;
Pointer = &Value; // указатель на Value по адресу в памяти
(*Pointer) = 12; // разыменование указателя, новое значение Value по указателю
int V1 = sizeof(Pointer); // размер указателя 8 байт
int V2 = sizeof(*Pointer); // разыменование указателя, размер переменной 4 байта
```

Pointer to an array
```C
teststruct Array[50];
teststruct *ArrayPointer = Array;
Array[25].Value = 10;
(ArrayPointer + 25)->Value = 20; // компилятор понимает, что при увеличении указателя мы перемещается по элементам массива
((teststruct *)((char *)ArrayPointer + 25 * sizeof(teststruct)))->Value = 30; // аналогично прошлому варианту, только мы передвигаемся по массиву побайтово
```


## Pointer to pointer

Пример использования указателя на указатель
```C
*(void **)&Code[7] = Five;
&Code[7] // получаем адрес переменной
(void **)&Code[7] // кастим адрес переменной к указателю на указатель
```

Указатель на указатель `int **ptr`, т.е. «ptr» указывает на указатель, который, в свою очередь, указывает на значение типа «int»

Пример использования указателя на указатель для обхода двумерного массива
```C
int arr[3][3] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} };
int **ptr = (int **)arr; // приведение типа двумерного массива к указателю на указатель
for (int i = 0; i < 3; i++)
{
    for (int j = 0; j < 3; j++)
    {
        printf("%d ", **ptr); // вывод значения, на которое указывает указатель на указатель
        ptr++; // переход к следующему указателю
    }
}
```

В данном примере две звездочки используются для работы с указателями на строки двумерного массива. Используя выражение *(array + i), мы получаем указатель на каждую строку массива. Затем, при помощи второй звездочки, мы обращаемся к каждому элементу строки и находим их сумму
```C
int array[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
int sum = 0;
int i, j;
for (i = 0; i < 3; i++)
{
    for (j = 0; j < 3; j++)
    {
        sum += **(array + i) + j;
    }
}
printf("Сумма всех элементов массива: %d", sum);
```

В данном примере объявляется переменная matrix, которая является указателем на указатель на int. С помощью функции malloc выделяется память под каждый элемент массива отдельно, что позволяет создать двумерный массив с заданным числом строк и столбцов. Т.е. размер отдельно взятой cols может быть любым
```C
int rows = 3;
int cols = 5;
int** matrix = (int**)malloc(rows * sizeof(int*));
for (int i = 0; i < rows; i++)
{
    matrix[i] = (int*)malloc(cols * sizeof(int));
}
```


## Function pointer

Указатель на функцию это такой же указатель, который показывает где в памяти находится первый байт функции, но в языке C мы не можем вызвать напрямую функцию возвращающую значение по указателю, а должны использовать typedef

В данном случае компилятор выведет ошибку, т.к. не сможет понять тип возвращаемого значения из-за ABI
```C
int Five(void)
{
    return(5);
}
void *Function = Five;
int Number = Function();
```

Для избежания ошибки описанной выше, необходимо строго указать сигнатуру функции, чтобы компилятор мог её понимать
```C
typedef int number_function(void);
int Five(void)
{
    return(5);
}
number_function *Function = Five;
int Number = Function();
```

Описанное выше мы можем использовать для определения платформонезависимой функции, т.е. сигнатура функции будет находиться в платформонезависимом файле, а реализация в файле платформы
```C
// macro
#define PLATFORM_TOGGLE_FULLSCREEN(name) void name(void)
typedef PLATFORM_TOGGLE_FULLSCREEN(platform_toggle_fullscreen);

// function realisation
PLATFORM_TOGGLE_FULLSCREEN(PlatformToggleFullscreen)
{
    ...
}

// function call
platform_toggle_fullscreen *ToggleFullscreen;
ToggleFullscreen();
```


## Pointer to array of structs

Trick to avoid modifying lots of code to reflect the change variable from object to pointer
```C
game_assets Assets_;
game_assets *Assets = &Assets_;
```