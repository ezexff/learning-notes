#if APP_INTERNAL
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>

#if APP_INTERNAL
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ImGuiIO *GlobalIO;

// NOTE(ezexff): ImGui windows
bool GlobalShowDemoWindow = true;
bool GlobalShowAnotherWindow = true;
#endif

bool GlobalRunning;
WINDOWPLACEMENT GlobalWindowPosition = {sizeof(GlobalWindowPosition)};
float GlobalClearColor[3] = {1.0f, 1.0f, 0.0f};

// NOTE(ezexff): Floating point fix for without CRT release compile
#if !(APP_INTERNAL)
extern "C"
{
    int _fltused;
}
#endif

void ErrorMessageBox(char *Message)
{
    MessageBoxA(0, Message, 0, MB_OK);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
                        UINT Message,
                        WPARAM WParam,
                        LPARAM LParam)
{
    // NOTE(ezexff): ImGui message handler
    {
#if APP_INTERNAL
        if (ImGui_ImplWin32_WndProcHandler(Window, Message, WParam, LParam))
        {
            return true;
        }
#endif
    }
    
    LRESULT Result = 0;
    
    switch(Message)
    {
        case WM_CLOSE:
        {
            OutputDebugStringA("WM_CLOSE\n");
            GlobalRunning = false;
        } break;
        
        case WM_DESTROY:
        {
            OutputDebugStringA("WM_DESTROY\n");
            GlobalRunning = false;
        } break;
        
        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }
    
    return(Result);
}

void
Win32InitOpenGL(HWND Window)
{
    HDC WindowDC = GetDC(Window);
    
    PIXELFORMATDESCRIPTOR DesiredPixelFormat = {};
    DesiredPixelFormat.nSize = sizeof(DesiredPixelFormat);
    DesiredPixelFormat.nVersion = 1;
    DesiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
    DesiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
    DesiredPixelFormat.cColorBits = 32;
    DesiredPixelFormat.cAlphaBits = 8;
    DesiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
    
    int SuggestedPixelFormatIndex = ChoosePixelFormat(WindowDC, &DesiredPixelFormat);
    PIXELFORMATDESCRIPTOR SuggestedPixelFormat;
    DescribePixelFormat(WindowDC, SuggestedPixelFormatIndex,
                        sizeof(SuggestedPixelFormat), &SuggestedPixelFormat);
    SetPixelFormat(WindowDC, SuggestedPixelFormatIndex, &SuggestedPixelFormat);
    
    HGLRC OpenGLRC = wglCreateContext(WindowDC);
    if(wglMakeCurrent(WindowDC, OpenGLRC))        
    {
        OutputDebugStringA("OpenGL context created\n");
    }
    else
    {
        ErrorMessageBox("Can't make current opengl context");
    }
    ReleaseDC(Window, WindowDC);
}

void
ToggleFullscreen(HWND Window)
{
    // NOTE(casey): This follows Raymond Chen's prescription
    // for fullscreen toggling, see:
    // http://blogs.msdn.com/b/oldnewthing/archive/2010/04/12/9994016.aspx
    
    DWORD Style = GetWindowLong(Window, GWL_STYLE);
    if(Style & WS_OVERLAPPEDWINDOW)
    {
        MONITORINFO MonitorInfo = {sizeof(MonitorInfo)};
        if(GetWindowPlacement(Window, &GlobalWindowPosition) &&
           GetMonitorInfo(MonitorFromWindow(Window, MONITOR_DEFAULTTOPRIMARY), &MonitorInfo))
        {
            SetWindowLong(Window, GWL_STYLE, Style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(Window, HWND_TOP,
                         MonitorInfo.rcMonitor.left, MonitorInfo.rcMonitor.top,
                         MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left,
                         MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    else
    {
        SetWindowLong(Window, GWL_STYLE, Style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(Window, &GlobalWindowPosition);
        SetWindowPos(Window, 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

void
Win32ProcessPendingMessages()
{
    MSG Message;
    while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        switch(Message.message)
        {
            case WM_QUIT:
            {
                GlobalRunning = false;
            } break;
            
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                unsigned int VKCode = (unsigned int)Message.wParam;
                
                // NOTE(casey): Since we are comparing WasDown to IsDown,
                // we MUST use == and != to convert these bit tests to actual
                // 0 or 1 values.
                bool WasDown = ((Message.lParam & (1 << 30)) != 0);
                bool IsDown = ((Message.lParam & (1 << 31)) == 0);
                if(WasDown != IsDown)
                {
                    if(VKCode == VK_UP)
                    {
                        OutputDebugStringA("VK_UP\n");
                    }
                    else if(VKCode == VK_LEFT)
                    {
                        OutputDebugStringA("VK_LEFT\n");
                    }
                    else if(VKCode == VK_DOWN)
                    {
                        OutputDebugStringA("VK_DOWN\n");
                    }
                    else if(VKCode == VK_RIGHT)
                    {
                        OutputDebugStringA("VK_RIGHT\n");
                    }
                    else if(VKCode == VK_ESCAPE)
                    {
                        OutputDebugStringA("VK_ESCAPE\n");
                        GlobalRunning = false;
                    }
                    else if(VKCode == VK_SPACE)
                    {
                        OutputDebugStringA("VK_SPACE\n");
                    }
                }
                
                if(IsDown)
                {
                    bool AltKeyWasDown = (Message.lParam & (1 << 29));
                    if((VKCode == VK_F4) && AltKeyWasDown)
                    {
                        GlobalRunning = false;
                    }
                    if((VKCode == VK_RETURN) && AltKeyWasDown)
                    {
                        if(Message.hwnd)
                        {
                            ToggleFullscreen(Message.hwnd);
                        }
                    }
                }
            } break;
            
            default:
            {
                TranslateMessage(&Message);
                DispatchMessageA(&Message);
            } break;
        }
    }
}

void UpdateAndRender()
{
    // NOTE(ezexff): Prepare ImGui frame to render
    {
#if APP_INTERNAL
        // Beginning of frame: update Renderer + Platform backend, start Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if(GlobalShowDemoWindow)
        {
            ImGui::ShowDemoWindow(&GlobalShowDemoWindow);
        }
        
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;
            
            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
            
            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &GlobalShowDemoWindow);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &GlobalShowAnotherWindow);
            
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&GlobalClearColor); // Edit 3 floats representing a color
            
            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);
            
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / GlobalIO->Framerate, GlobalIO->Framerate);
            //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        
        // 3. Show another simple window.
        if(GlobalShowAnotherWindow)
        {
            ImGui::Begin("Another Window", &GlobalShowAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
            {
                GlobalShowAnotherWindow = false;
            }
            ImGui::End();
        }
#endif
    }
    
    // NOTE(ezexff): OpenGL render
    {
        glClearColor(GlobalClearColor[0], GlobalClearColor[1], GlobalClearColor[2], 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glColor3f(1, 0, 0);
        glBegin(GL_QUADS);
        
        float OffsetFromCenter = 0.5f;
        glVertex2f(-OffsetFromCenter, -OffsetFromCenter);
        glVertex2f(OffsetFromCenter, -OffsetFromCenter);
        glVertex2f(OffsetFromCenter, OffsetFromCenter);
        glVertex2f(-OffsetFromCenter, OffsetFromCenter);
        
        glEnd();
    }
}

#if APP_INTERNAL
int main(int, char**)
#else
extern "C" void __stdcall WinMainCRTStartup(void)
#endif
{
    HINSTANCE Instance = GetModuleHandle(0);
    
    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
    // WindowClass.hIcon = ;
    WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
    WindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    // WindowsClass.lpszMenuName = ;
    WindowClass.lpszClassName = "ImGuiOpenGLAppWindowClass";
    
    if(RegisterClassA(&WindowClass))
    {
        HWND Window =
            CreateWindowExA(
                            0, // WS_EX_TOPMOST|WS_EX_LAYERED,
                            WindowClass.lpszClassName,
                            "Windows OpenGL+ImGui App",
                            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            0,
                            0,
                            Instance,
                            0);
        if(Window)
        {
            Win32InitOpenGL(Window);
            
            // NOTE(ezexff): OpenGL version in window title
            {
                char *TitlePrefix = "OPENGL VERSION: ";
                char *TitleSuffix = (char *)glGetString(GL_VERSION);
                char Title[128];
                for(int Index = 0;
                    Index < sizeof(Title);
                    Index++)
                {
                    if(*TitlePrefix != 0)
                    {
                        Title[Index] = *TitlePrefix;
                        TitlePrefix++;
                    }
                    else
                    {
                        Title[Index] = *TitleSuffix;
                        if(*TitleSuffix == 0)
                        {
                            break;
                        }
                        TitleSuffix++;
                    }
                }
                SetWindowTextA(Window, Title);
            }
            
            // NOTE(ezexff): Init ImGui
            {
#if APP_INTERNAL
                // Create a Dear ImGui context, setup some options
                IMGUI_CHECKVERSION();
                ImGui::CreateContext();
                GlobalIO = &ImGui::GetIO();
                GlobalIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
                
                // Initialize Platform + Renderer backends
                ImGui_ImplWin32_InitForOpenGL(Window);
                ImGui_ImplOpenGL3_Init();
#endif
            }
            
            GlobalRunning = true;
            
            while(GlobalRunning)
            {
                Win32ProcessPendingMessages();
                
                // NOTE(ezexff): Setting up render area
                {
                    RECT CRect;
                    int CWidth = 960;
                    int CHeight = 540;
                    if(GetClientRect(Window, &CRect))
                    {
                        CWidth = CRect.right - CRect.left;
                        CHeight = CRect.bottom - CRect.top;
                    }
                    glViewport(0, 0, CWidth, CHeight);
                }
                
                UpdateAndRender();
                
                // NOTE(ezexff): Render ImGui frame
                {
#if APP_INTERNAL
                    // End of frame: render Dear ImGui
                    ImGui::Render();
                    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
                }
                
                HDC DeviceContext = GetDC(Window);
                SwapBuffers(DeviceContext);
                ReleaseDC(Window, DeviceContext);
            }
        }
        else
        {
            ErrorMessageBox("Can't create window");
        }
    }
    else
    {
        ErrorMessageBox("Can't register window class");
    }
    
    // NOTE(ezexff): Destroy ImGui
    {
#if APP_INTERNAL
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
#endif
    }
    
    ExitProcess(0);
}