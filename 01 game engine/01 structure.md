# Project structure

Файловая организация структуры проекта - три модуля или три единицы трансляции
<table>
    <tr>
        <th>Source</th>
        <th>Dest</th>
        <th>Description</th>
    </tr>
    <tr>
        <td>win32_engine.cpp</td>
        <td>.exe</td>
        <td>win32 слой платформы, реализующий взаимодействие с ОС</td>
    </tr>
    <tr>
        <td>win32_engine_opengl.cpp</td>
        <td>.dll</td>
        <td>win32 рендерер через OpenGL</td>
    </tr>
    <tr>
        <td>engine.cpp</td>
        <td>.dll</td>
        <td>платформонезависимый код игры</td>
    </tr>
</table>

Взаимосвязи между единицами трансляции
* win32_engine.cpp (.exe) - import functions from engine and renderer .dll's
  * GameUpdateAndRender()
  * GameGetSoundSamples()
  * Win32LoadRenderer()
  * Win32BeginFrame()
  * Win32EndFrame()
* win32_engine_opengl.cpp (.dll) - export functions
  * Win32LoadRenderer()
  * Win32BeginFrame()
  * Win32EndFrame()
* engine.cpp (.dll) - export functions
  * GameUpdateAndRender()
  * GameGetSoundSamples()

Минимальная структура релизной версии игры на Windows, где data*.eab это файл с ассетами
* win32_engine.exe
* win32_engine_opengl.dll
* engine.dll 
* data*.eab

Файловая структура с реализацией альтернативных графических API
* win32_engine.exe
* win32_engine_opengl.dll
* win32_engine_direct3d.dll
* win32_engine_vulkan.dll
* engine.dll
* data*.eab

Файловая структура порта на Linux
* linux_engine.exe
* linux_engine_vulkan.dll
* engine.dll 
* data*.eab

## Code structure

Пример организация текста программы в модулях

Platform Layer (win32_engine.exe) - WinMain() entry point
* Init work queues (multithreaded tasks)
* Init performance counter (fps lock)
* Init dynamic libraries (.dll's)
* Init pixels buffer
* Init Windows window - CreateWindowExA()
  * Init Audio with DirectSound and VirtuaAlloc() for playing samples
  * Init memory VirtualAlloc()
    * PermanentStorage - VirtualAlloc() 256 Megabytes
    * TransientStorage - VirtualAlloc() 1 Gigabyte
  * Init replay buffers (for live code loop editing)
  * 1st frame QueryPerformanceCounter()
  * Main Loop 
    * Load game code from .dll (only when .dll changed)
    * Process keyboard inputs
    * Process mouse inputs (cursor pos and buttons)
    * Process gamepad inputs with XInput
    * Record and play inputs in file (for live code loop editing)
    * Call Game Layer function - GameUpdateAndRender()
    * Setting up Audio processing (detection audio card latency and write, play cursor pos)
    * Call GetSoundSamples()
    * Fill Sound buffer
    * Counter for work on current frame - target FPS with QueryPerformanceCounter()
    * Sleep() for target FPS
    * Display pixels buffer
    * Counter for flip frame QueryPerformanceCounter() for audio sync

Renderer Layer (win32_engine_opengl.dll)
* Export function LoadRenderer() - init renderer context
* Export function BeginFrame() - prepare push buffer for drawing
* Export function EndFrame() - draw push buffer

Game Layer (engine.dll)
* Export function GameUpdateAndRender()
  * Init PermanentStorage (once on startup)
    * Init and generate world
    * Init collisions
    * Init camera
  * Init TransientStorage (once on startup)
    * Init HighPriorityQueue and LowPriorityQueue (multithreaded tasks)
    * Init Assets (load from hard drive to RAM)
    * Init GroundBuffers (for rendering ground pieces only around camera)
  * Process Inputs
  * Init render queue in memory
  * BeginSim() function - getting entities, that inside simulated region bounds, from static memory
  * Update entities that in simulated region bounds (update movements, physics, collisions and etc.) and putting to render queue
  * Draw from render queue
  * EndSim() function - putting updated entities, that inside simulated region bounds, into static memory
* Export function GetSoundSamples()
  * Init temporary memory for mixer
  * Sum all playing sounds
  * Convert channels to 16-bit
  * Free temporary memory for mixer