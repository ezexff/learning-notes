# C/C++

MIT (Massachusetts Institute of Technology) - Introductory Programming - неплохие курсы для новичков

Программирование на языке C наиболее приближено к машинному уровню

Почти все нововведения C++ плохие или бесполезные. Например, виртуальные функции плохие, т.к. их поведение не может контролировать программист

Полезными в С++ являются перегрузка операторов и функций

Описание параметров `int main(int argc, char* argv[])`
* `int argc` - argument count - это количество аргументов командной строки, минимально 1
* `char* argv[]` - `argv[0]` это имя программного файла, а `argv[1 to N]` переданные аргументы

Логические операторы
<table>
    <tr>
        <th>C/C++ symbol</th>
        <th>Meaning</th>
        <th>Description</th>
    </tr>
    <tr>
        <td>&</td>
        <td>bitwise AND</td>
        <td>takes two numbers as operands and does AND on every bit of two numbers. The result of AND is 1 only if both bits are 1</td>
    </tr>
    <tr>
        <td>|</td>
        <td>bitwise OR</td>
        <td>takes two numbers as operands and does OR on every bit of two numbers. The result of OR is 1 if any of the two bits is 1</td>
    </tr>
    <tr>
        <td>^</td>
        <td>bitwise XOR</td>
        <td>takes two numbers as operands and does XOR on every bit of two numbers. The result of XOR is 1 if the two bits are different</td>
    </tr>
    <tr>
        <td><<</td>
        <td>left shift</td>
        <td>takes two numbers as operands and does XOR on every bit of two numbers. The result of XOR is 1 if the two bits are different</td>
    </tr>
    <tr>
        <td>>></td>
        <td>right shift</td>
        <td>takes two numbers, right shifts the bits of the first operand, and the second operand decides the number of places to shift</td>
    </tr>
    <tr>
        <td>~</td>
        <td>bitwise NOT</td>
        <td>takes one number and inverts all bits of it</td>
    </tr>
</table>

Проверка C макроса в MSVC
> cl /EP handmade.cpp > preprocess.tmp

Функция atoi (ASCII to integer, из ASCII в целое число) в языке программирования Си используется для приведения (конвертации) строки в числовой вид.