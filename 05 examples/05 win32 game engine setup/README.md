# Win32 game engine setup

Число строк кода данного проекта `1656` и суммарный размер скомпилированных файлов `30720` байт (без SVML 15872 байт). Несмотря на очень маленькое число строк кода и крошечный размер скомплированных файлов, в данной заготовке есть всё необходимое для создания игры. Реализованы все важнейшие подсистемы игрового движка
* Bat for build debug and release versions of program
* Platform specific Windows code
  * `win32_engine.exe` - window creation, keys processing, memory init, file io
  * `win32_engine_opengl.dll` - renderer
* Platform-independent code
  * `engine.dll` - game and audio mixer
* Debug ImGUI windows
* Timers
* Locked FPS
* Keyboard inputs processing
* Push buffer renderer
* Memory allocator
  * Permanent storage
  * Transient storage
* Reading file groups
* WASAPI implementation
* Import WAV file
* Simple sound mixer
* SIMD intrinsics for trigonometric math functions (SVML)
* `LOC = 1656` (lines of code counted by [cloc](https://github.com/AlDanial/cloc))

Debug version
<table>
    <tr>
        <th>Name</th>
        <th>Size (bytes)</th>
    </tr>
    <tr>
        <td>win32_engine.exe</td>
        <td>1 783 296</td>
    </tr>
    <tr>
        <td>win32_engine_opengl.dll</td>
        <td>1 763 840</td>
    </tr>
    <tr>
        <td>engine.dll</td>
        <td>1 781 248</td>
    </tr>
</table>

Release version
<table>
    <tr>
        <th>Name</th>
        <th>Size (bytes)</th>
    </tr>
    <tr>
        <td>win32_engine.exe</td>
        <td>8 704</td>
    </tr>
    <tr>
        <td>win32_engine_opengl.dll</td>
        <td>4 096</td>
    </tr>
    <tr>
        <td>engine.dll</td>
        <td>17 920 (3 072 no SVML)</td>
    </tr>
</table>