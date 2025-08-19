#include <windows.h>
#include <gl/gl.h>

bool GlobalRunning;
WINDOWPLACEMENT GlobalWindowPosition = {sizeof(GlobalWindowPosition)};

// NOTE(ezexff): Floating point fix
extern "C"
{
    int _fltused;
}

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

extern "C" void __stdcall WinMainCRTStartup(void)
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
    WindowClass.lpszClassName = "OpenGLAppWindowClass";
    
    if(RegisterClassA(&WindowClass))
    {
        HWND Window =
            CreateWindowExA(
                            0, // WS_EX_TOPMOST|WS_EX_LAYERED,
                            WindowClass.lpszClassName,
                            "Windows OpenGL App",
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
            
            GlobalRunning = true;
            
            while(GlobalRunning)
            {
                Win32ProcessPendingMessages();
                
                // NOTE(ezexff): Getting window rect for testing and setting up render area
                {
                    RECT WRect;
                    int WWidth = 0;
                    int WHeight = 0;
                    if(GetWindowRect(Window, &WRect))
                    {
                        WWidth = WRect.right - WRect.left;
                        WHeight = WRect.bottom - WRect.top;
                    }
                    
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
                
                // NOTE(ezexff): OpenGL render
                {
                    glClearColor(1.0f, 1.0f, 0.0f, 0.0f);
                    glClear(GL_COLOR_BUFFER_BIT);
                    
                    glColor3f(1, 0, 0);
                    glBegin(GL_QUADS);
                    
                    float OffsetFromCenter = 0.5f;
                    glVertex2f(-OffsetFromCenter, -OffsetFromCenter);
                    glVertex2f(OffsetFromCenter, -OffsetFromCenter);
                    glVertex2f(OffsetFromCenter, OffsetFromCenter);
                    glVertex2f(-OffsetFromCenter, OffsetFromCenter);
                }
                
                glEnd();
                
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
    
    ExitProcess(0);
}