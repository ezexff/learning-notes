# Win32 no CRT console app

* Endianness check
* Array and value pointers
* Pointer on function call
* Function call in bytecodes
* Function call in bytecodes with executable flag
* Function in bytecodes
* Console app who get and output .exe options

Debug version
<table>
    <tr>
        <th>Name</th>
        <th>Size CRT (bytes)</th>
        <th>Size no CRT (bytes)</th>
    </tr>
    <tr>
        <td>win32_main.exe</td>
        <td>353 792</td>
        <td>3 072</td>
    </tr>
</table>

Release version
<table>
    <tr>
        <th>Name</th>
        <th>Size CRT (bytes)</th>
        <th>Size no CRT (bytes)</th>
    </tr>
    <tr>
        <td>win32_main.exe</td>
        <td>110 080</td>
        <td>2 560</td>
    </tr>
</table>
