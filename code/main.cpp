
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
 
#include "defines.h"

#include "render.cpp"

#define _CRT_SECURE_NO_WARNINGS
#undef UNICODE
#include <Windows.h>

// TODO: Better Logging
#define LOG_ERROR(str) LogError(__FILE__, __LINE__, str)
#define LOG_MESSAGE(str) fprintf(stdout, "MESSAGE: %s line %d: %s\n", __FILE__, __LINE__, str)

void LogError(char* file, int line, char* str){
    fprintf(stderr, "ERROR: %s line %d: %s\n    ERRNO:%s\n", file, line, str, strerror(errno));
}

struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
};

struct win32_window_dimensions
{
    int Width;
    int Height;
};

global bool Running;
global win32_offscreen_buffer BackBuffer;
global bool OutputDebugInfo;

internal win32_window_dimensions Win32GetWindowDimension
(HWND Window)
{
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    
    win32_window_dimensions Dimensions;
    Dimensions.Width = ClientRect.right - ClientRect.left;
    Dimensions.Height = ClientRect.bottom - ClientRect.top;
    
    return Dimensions;
}

internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
    // TODO(casey): Bulletproof this.
    // Maybe don't free first, free after, then free first if that fails.

    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Width = Width;
    Buffer->Height = Height;

    int BytesPerPixel = 4;

    // NOTE(casey): When the biHeight field is negative, this is the clue to
    // Windows to treat this bitmap as top-down, not bottom-up, meaning that
    // the first three bytes of the image are the color for the top left pixel
    // in the bitmap, not the bottom left!
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    // NOTE(casey): Thank you to Chris Hecker of Spy Party fame
    // for clarifying the deal with StretchDIBits and BitBlt!
    // No more DC for us.
    int BitmapMemorySize = (Buffer->Width*Buffer->Height)*BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    Buffer->Pitch = Width*BytesPerPixel;

    // TODO(casey): Probably clear this to black
}

internal void Win32DisplayBuffer
(HDC DeviceContext, int WindowWidth, int WindowHeight, win32_offscreen_buffer *Buffer)
{
    SetStretchBltMode(DeviceContext, STRETCH_DELETESCANS);
    StretchDIBits(
        DeviceContext,
        0, 0, WindowWidth, WindowHeight,
        0, 0, Buffer->Width, Buffer->Height,
        Buffer->Memory, &Buffer->Info,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}


LRESULT CALLBACK Win32MainWindowCallback
(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = 0;
    
    switch(Message){
        case WM_SIZE:
        {
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(&BackBuffer, Width, Height);
        } break;
        
        case WM_DESTROY:
        {
            Running = false;
        }break;
        
        case WM_CLOSE:
        {
            Running = false;
        }break;
        
        case WM_ACTIVATEAPP:
        {
            
        }break;
        
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:{
            uint32 VKCode = wParam;
            bool WasDown = (lParam & (1 << 30)) != 0;
            bool IsDown = (lParam & (1 << 31)) == 0;
            
            if(IsDown != WasDown)
            {
                switch(VKCode)
                {
                    case 'W':
                    {
                    }break;
                    case 'A':
                    {
                    }break;
                    case 'S':
                    {
                    }break;
                    case 'D':
                    {
                    }break;
                    case 'Q':
                    {
                    }break;
                    case 'E':
                    {
                    }break;
                    case VK_UP:
                    {
                    }break;
                    case VK_DOWN:
                    {
                    }break;
                    case VK_LEFT:
                    {
                    }break;
                    case VK_RIGHT:
                    {
                    }break;
                    case VK_ESCAPE:
                    {
                    }break;
                    case VK_SPACE:
                    {
                        if(OutputDebugInfo)
                        {
                            OutputDebugInfo = false;
                        }
                        else
                        {
                            OutputDebugInfo = true;
                        }
                    }break;
                    
                }
                bool AltKeyDown = ((lParam & (1 << 29)) != 0);
                if((VKCode == VK_F4) && AltKeyDown)
                {
                    Running = false;
                }
            }
        }break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            win32_window_dimensions Dimension = Win32GetWindowDimension(Window);
            Win32DisplayBuffer(DeviceContext, Dimension.Width, Dimension.Height, &BackBuffer);
            EndPaint(Window, &Paint);
        }break;
        
        default:
        {
            Result = DefWindowProc(Window, Message, wParam, lParam);
        }break;
    }
    return(Result);
}

int CALLBACK WinMain
(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCode)
{
    WNDCLASS WindowClass = {};
    WindowClass.style = /*CS_HREDRAW|CS_VREDRAW|*/CS_OWNDC;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
    //WindowClass.hIcon = ;
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";
    
    HWND Window;
    
    BackBuffer.Width = 1920;
    BackBuffer.Height = 1080;
    
    BackBuffer.Info.bmiHeader.biSize = sizeof(BackBuffer.Info.bmiHeader);
    BackBuffer.Info.bmiHeader.biWidth = BackBuffer.Width;
    BackBuffer.Info.bmiHeader.biHeight = -BackBuffer.Height; // Negative to make bitmap top-down
    BackBuffer.Info.bmiHeader.biPlanes = 1;
    BackBuffer.Info.bmiHeader.biBitCount = 32;
    BackBuffer.Info.bmiHeader.biCompression = BI_RGB;
    
    int BytesPerPixel = 4;
    int BackBufferMemorySize = (BackBuffer.Height*BackBuffer.Width)*BytesPerPixel;
    
    BackBuffer.Memory = VirtualAlloc(NULL, BackBufferMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    BackBuffer.Pitch = BackBuffer.Width*BytesPerPixel;
    
    if (!AllocConsole()){OutputDebugString("Console Alloc Failed\n");}
    HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleMode(ConsoleHandle, ENABLE_PROCESSED_OUTPUT|ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    
    if(!RegisterClass(&WindowClass)){return -1;}
    
    Window = CreateWindowEx(
        0,
        WindowClass.lpszClassName,
        "Handmade Hero",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        0, 0, Instance, 0
    );
    
    if(!Window){return -1;}
    
    HDC DeviceContext = GetDC(Window);
    
    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency(&PerfCountFrequencyResult);
    int64 PerfCountFrequency = PerfCountFrequencyResult.QuadPart;
    
    LARGE_INTEGER LastCounter;
    QueryPerformanceCounter(&LastCounter);
    uint64 LastCycleCount = __rdtsc();
    
    LARGE_INTEGER StartCounter;
    QueryPerformanceCounter(&StartCounter);
    uint64 StartCycleCount = __rdtsc();
    
    LARGE_INTEGER TimeSinceStart;
    TimeSinceStart.QuadPart = 0;
    
    Running = true;
    OutputDebugInfo = true;
    while(Running)
    {
        
        MSG Message;
        while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE)){
            if(Message.message == WM_QUIT){
                Running = false;
            }
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        
        persist int XOffset = 0;
        persist int YOffset = 0;
        XOffset = (double)TimeSinceStart.QuadPart / (double)PerfCountFrequency * 100.0;
        
        screen_buffer ScreenBuffer = {0};
        ScreenBuffer.Memory = BackBuffer.Memory;
        ScreenBuffer.Height = BackBuffer.Height;
        ScreenBuffer.Width = BackBuffer.Width;
        ScreenBuffer.Pitch = BackBuffer.Pitch;
        
        RenderScreen(&ScreenBuffer);
        
        win32_window_dimensions Dimension = Win32GetWindowDimension(Window);
        Win32DisplayBuffer(DeviceContext, Dimension.Width, Dimension.Height, &BackBuffer);
        
        LARGE_INTEGER EndCounter;
        QueryPerformanceCounter(&EndCounter);
        
        int64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
        
        uint64 EndCycleCount = __rdtsc();
        
        int64 CyclesElapsed = EndCycleCount - LastCycleCount;
        LastCycleCount = EndCycleCount;
        
        TimeSinceStart.QuadPart = EndCounter.QuadPart - StartCounter.QuadPart;
        
        float MSPF = ((float)CounterElapsed * 1000.0f) / (float)PerfCountFrequency;
        float FPS = 1000.0f / MSPF;
        float MCPS = (float)CyclesElapsed / 1000.0f /1000.0f;
        if(OutputDebugInfo)
        {
            printf("\033[H\033[J");
            printf("ms: %05.2f  fps: %06.2f  cycles(M): %05.2f\n", MSPF, FPS, MCPS);
        }
        LastCounter = EndCounter;
    }
    
    return(0);
}