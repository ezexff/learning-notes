# Win32 no CRT desktop app and dynamic-link library

* Fixes for x64 no CRT setup
* Call .dll functions
* Output in MessageBox

Debug version
<table>
    <tr>
        <th>Name</th>
        <th>Size CRT (bytes)</th>
        <th>Size no CRT (bytes)</th>
    </tr>
    <tr>
        <td>win32_main.exe</td>
        <td>343 552</td>
        <td>4 096</td>
    </tr>
    <tr>
        <td>win32_library.dll</td>
        <td>339 456</td>
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
        <td>106 496</td>
        <td>3 072</td>
    </tr>
    <tr>
        <td>win32_library.dll</td>
        <td>105 472</td>
        <td>2 048</td>
    </tr>
</table>
