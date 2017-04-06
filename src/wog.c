/**
* Copyright (c) 2015~2017 chai(neonum)
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the MIT license. See LICENSE for details.
*/

#include "wog.h"
#include <Windows.h>
#include <malloc.h>
#include <stdio.h>
#include <gl/GL.h>
#include <gl/GLU.h>

/**
* We need opengl32.lib and glu32.lib for using opengl functions on 
* windows platform. 
*/
#pragma comment( lib, "opengl32.lib" )                                        
#pragma comment( lib, "glu32.lib" )                                            

#define WINDOW_CLASS "OGL_WND"

#define heap_alloc(T, C)  (T*)malloc(sizeof(T)*(C))
#define stack_alloc(T, C) (T*)alloca(sizeof(T)*(C))
#define zero_mem(I, L)    memset(I, 0, L)

#define max(a, b) (a < b ? b : a) 
#define min(a, b)  (a < b ? a : b)  
#define clamp(x, minv, maxv)  (max(minv, min(x, maxv)))


typedef struct wog_Window
{
    HWND  hwnd; // Window handler 
    HDC   hdc;  // Device Context
}wog_Window;


typedef struct wog_GLContext
{
    HGLRC hrc; // Rendering Context
}wog_GLContext;


void wog_handleEvent(wog_Window* window, MSG* msg, wog_Event* e)
{
    UINT uMsg;
    WPARAM wParam;
    LPARAM lParam;
    uMsg = msg->message;
    wParam = msg->wParam;
    lParam = msg->lParam;
    HWND hWnd = window->hwnd;

    switch (uMsg)
    {
    case WM_SYSCOMMAND:
    {
        switch (wParam)
        {
        case SC_SCREENSAVE:
        case SC_MONITORPOWER:
            return 0;
        }
        break;
    }
    return 0;

    case WM_CLOSE:
    {
        zero_mem(e, sizeof(wog_Event));
        e->type = WOG_ECLOSE;
        return 0;
    }

    case WM_KEYDOWN:
        if ((wParam >= 0) && (wParam <= 255))
        {
            zero_mem(e, sizeof(wog_Event));
            e->type = WOG_EKEYDOWN;
            e->key = wParam;

            return 0;
        }
        break;

    case WM_KEYUP:
        if ((wParam >= 0) && (wParam <= 255))
        {
            zero_mem(e, sizeof(wog_Event));
            e->type = WOG_EKEYUP;
            e->key = wParam;

            return 0;
        }
        break;

    case WM_MOUSEMOVE:
    {
        zero_mem(e, sizeof(wog_Event));
        e->type = WOG_EMOUSEMOTION;
        wog_getMouse(window, &e->pos.x, &e->pos.y);

        return 0;
    }

    case WM_MOUSEWHEEL: // mousewheel scroll 
    {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        zero_mem(e, sizeof(wog_Event));
        e->type = WOG_EMOUSEWHEEL;
        e->wheel = zDelta > 0 ? 1 : -1;
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        zero_mem(e, sizeof(wog_Event));
        e->type = WOG_EMOUSEBUTTONDOWN;
        e->button = WOG_MOUSE_LBUTTON;
        return 0;
    }

    case WM_RBUTTONDOWN:
    {
        zero_mem(e, sizeof(wog_Event));
        e->type = WOG_EMOUSEBUTTONDOWN;
        e->button = WOG_MOUSE_RBUTTON;
        return 0;
    }

    case WM_MBUTTONDOWN:
    {
        zero_mem(e, sizeof(wog_Event));
        e->type = WOG_EMOUSEBUTTONDOWN;
        e->button = WOG_MOUSE_MIDDLE;
        return 0;
    }

    case WM_LBUTTONUP:
    {
        zero_mem(e, sizeof(wog_Event));
        e->type = WOG_EMOUSEBUTTONUP;
        e->button = WOG_MOUSE_LBUTTON;
        return 0;
    }

    case WM_RBUTTONUP:
    {
        zero_mem(e, sizeof(wog_Event));
        e->type = WOG_EMOUSEBUTTONUP;
        e->button = WOG_MOUSE_RBUTTON;
        return 0;
    }

    case WM_MBUTTONUP:
    {
        zero_mem(e, sizeof(wog_Event));
        e->type = WOG_EMOUSEBUTTONUP;
        e->button = WOG_MOUSE_MIDDLE;
        return 0;
    }

    break;

    default:
        e->type = WOG_EUNKNOWN;
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


int wog_pollEvent(wog_Window* wnd, wog_Event* e)
{
    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT) return 0;
        TranslateMessage(&msg);
        wog_handleEvent(wnd, &msg, e);
        return 1;
    }
    return 0;
}


// on size changed callback;
static wog_Callback onSizeChanged = 0;
static wog_Callback onQuit = 0;


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Get The Window Context
    wog_Window* window = (wog_Window*)(GetWindowLong(hWnd, GWL_USERDATA));

    // call callback functions 
#define call(callback)\
    if (callback) \
        callback(window) 

    switch (uMsg)
    {
    case WM_CREATE:
    {
        CREATESTRUCT* creation = (CREATESTRUCT*)(lParam);  // Store Window Structure Pointer
        window = (wog_Window*)(creation->lpCreateParams);  // Get wog_Window
        SetWindowLong(hWnd, GWL_USERDATA, (LONG)(window)); // Save it 
    }
    return 0;

    case WM_CLOSE:                                         // Post WM_CLOSE to message queue. 
        PostMessage(hWnd, WM_CLOSE, wParam, lParam);
        return 0;

    case WM_SIZE:
        call(onSizeChanged);
        return 0;

    case WM_QUIT:
        call(onQuit);
        return 0;

    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
#undef call
}


void wog_registerResizeCallback(wog_Callback cal)
{
    onSizeChanged = cal;
}


void wog_registerQuitCallback(wog_Callback cal)
{
    onQuit = cal; 
}


static int registerWindowClass()
{
    // Register A Window Class
    WNDCLASSEX windowClass;                                         // Window Class
    zero_mem(&windowClass, sizeof(WNDCLASSEX));                     // Make Sure Memory Is Cleared

    windowClass.cbSize        = sizeof(WNDCLASSEX);                 // Size Of The windowClass Structure
    windowClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // Redraws The Window For Any Movement / Resizing
    windowClass.lpfnWndProc   = (WNDPROC)(WindowProc);              // WindowProc Handles Messages
    windowClass.hInstance     = GetModuleHandle(0);                 // Set The Instance
    windowClass.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE);       // Class Background Brush Color
    windowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);        // Load The Arrow Pointer
    windowClass.lpszClassName = WINDOW_CLASS;                       // Sets The Applications Classname

    if (RegisterClassEx(&windowClass) == 0)                         // Did Registering The Class Fail?
    {
        // NOTE: Failure, Should Never Happen
        MessageBox(HWND_DESKTOP, "RegisterClassEx Failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;                                               // Return False (Failure)
    }
    return TRUE;
}


wog_Window* wog_createWindow(const char* title, int width, int height, int x, int y, uint32 flags)
{

    if (! registerWindowClass())
    {
        printf("Register window class failed.\n");
        return -1;
    }

    wog_Window* wnd = heap_alloc(wog_Window, 1);
    zero_mem(wnd, sizeof(wog_Window));

    DWORD windowStyle = 0;     // Define Our Window Style
    windowStyle |= WS_POPUP;
    windowStyle |= WS_OVERLAPPED;
    windowStyle |= WS_CAPTION; 
    windowStyle |= WS_SYSMENU; 
    windowStyle |= WS_MINIMIZEBOX;
    windowStyle |= WS_VISIBLE;
#define hasbit(fs, f) ((fs & f) == f)
    if (hasbit(flags, WOG_WND_RESIZABLE)) windowStyle |= WS_SIZEBOX; 
    if (hasbit(flags, WOG_WND_DISABLE)) windowStyle |= WS_DISABLED;

    DWORD windowExtendedStyle = WS_EX_APPWINDOW; // Define The Window's Extended Style

    PIXELFORMATDESCRIPTOR pfd =           // pfd Tells Windows How We Want Things To Be
    {
        sizeof(PIXELFORMATDESCRIPTOR), // Size Of This Pixel Format Descriptor
        1,                             // Version Number
        PFD_DRAW_TO_WINDOW |           // Format Must Support Window
        PFD_SUPPORT_OPENGL |           // Format Must Support OpenGL
        PFD_DOUBLEBUFFER,              // Must Support Double Buffering
        PFD_TYPE_RGBA,                 // Request An RGBA Format
        32,                            // Select Our Color Depth
        0, 0, 0, 0, 0, 0,              // Color Bits Ignored
        0,                             // No Alpha Buffer
        0,                             // Shift Bit Ignored
        0,                             // No Accumulation Buffer
        0, 0, 0, 0,                    // Accumulation Bits Ignored
        16,                            // 16Bit Z-Buffer (Depth Buffer)  
        0,                             // No Stencil Buffer
        0,                             // No Auxiliary Buffer
        PFD_MAIN_PLANE,                // Main Drawing Layer
        0,                             // Reserved
        0, 0, 0                        // Layer Masks Ignored
    };
    RECT windowRect = { 0, 0, width, height };
    AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);
    width = windowRect.right - windowRect.left;
    height = windowRect.bottom - windowRect.top;
    wnd->hwnd = CreateWindowEx(
        windowExtendedStyle,
        WINDOW_CLASS,
        title, 
        windowStyle, 
        x, y, 
        width, height,
        HWND_DESKTOP, 
        0, 
        GetModuleHandle(0), 
        wnd
        );

    if (wnd->hwnd == 0)
        return 0;

    if (hasbit(flags, WOG_WND_HIDDEN)) wog_hide(wnd);

    // init device context 
    wnd->hdc = GetDC(wnd->hwnd); 
    if (wnd->hdc== 0)
    {
        DestroyWindow(wnd->hdc);
        wnd->hwnd = 0; 
        return 0;
    }

    // set pixel format 
    unsigned int pixelformat; 
    pixelformat = ChoosePixelFormat(wnd->hdc, &pfd);
    if (pixelformat == 0)                                    
    {
        wog_destroyWindow(wnd);
        return 0;
    }

    // set pixel format 
    if (SetPixelFormat(wnd->hdc, pixelformat, &pfd) == 0 )
    {
        wog_destroyWindow(wnd); 
        return 0; 
    }

    return wnd;
}


wog_GLContext* wog_createGLContext(wog_Window* wnd)
{
    wog_GLContext* cxt = heap_alloc(wog_GLContext, 1);
    zero_mem(cxt, sizeof(wog_GLContext));
    cxt->hrc = wglCreateContext(wnd->hdc); // create opengl context base on device context 
    if (cxt->hrc == 0)
    {
        free(cxt);
        return 0;
    }
    return cxt; 
}


int wog_makeCurrent(wog_Window* wnd, wog_GLContext* cxt)
{
    if (wnd && cxt)
    {
        if (wglMakeCurrent(wnd->hdc, cxt->hrc) == 0)
        {
            return 0; 
        }    
        return 1; 
    }

    return 0; 
}


void wog_destroyGLContext(wog_GLContext* cxt)
{
    if (cxt && cxt->hrc)
    {
        wglDeleteContext(cxt->hrc);
        free(cxt);
    }
}


void wog_destroyWindow(wog_Window* wnd)
{
    if (wnd)
    {
        ReleaseDC(wnd->hwnd, wnd->hdc);
        DestroyWindow(wnd->hwnd);
        free(wnd);
    }
}


static void UnEscapeQuotes(char *arg)
{
    char *last = NULL;

    while (*arg) {
        if (*arg == '"' && *last == '\\') {
            char *c_curr = arg;
            char *c_last = last;

            while (*c_curr) {
                *c_last = *c_curr;
                c_last = c_curr;
                c_curr++;
            }
            *c_last = '\0';
        }
        last = arg;
        arg++;
    }
}


/* Parse a command line buffer into arguments */
static int ParseCommandLine(char *cmdline, char **argv)
{
    char *bufp;
    char *lastp = NULL;
    int argc, last_argc;

    argc = last_argc = 0;
    for (bufp = cmdline; *bufp; ) {
        /* Skip leading whitespace */
        while (isspace(*bufp)) {
            ++bufp;
        }
        /* Skip over argument */
        if (*bufp == '"') {
            ++bufp;
            if (*bufp) {
                if (argv) {
                    argv[argc] = bufp;
                }
                ++argc;
            }
            /* Skip over word */
            while (*bufp && (*bufp != '"' || (lastp && *lastp == '\\'))) {
                lastp = bufp;
                ++bufp;
            }
        }
        else {
            if (*bufp) {
                if (argv) {
                    argv[argc] = bufp;
                }
                ++argc;
            }
            /* Skip over word */
            while (*bufp && !isspace(*bufp)) {
                ++bufp;
            }
        }
        if (*bufp) {
            if (argv) {
                *bufp = '\0';
            }
            ++bufp;
        }

        /* Strip out \ from \" sequences */
        if (argv && last_argc != argc) {
            UnEscapeQuotes(argv[last_argc]);
        }
        last_argc = argc;
    }
    if (argv) {
        argv[argc] = NULL;
    }
    return(argc);
} 


#ifdef main 
#undef main 
#endif


#if defined(_MSC_VER) && !defined(_WIN32_WCE)
/* The VC++ compiler needs main defined */
#define console_main main
#endif


/**
* Entry of console application.
*/
int console_main(int argc, char* argv[])
{
    int status = wog_main(argc, argv); 
    return status;
}


/**
* Entry of windows application. 
*/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
    char* temp = GetCommandLine();
    int len = strlen(temp) + 1; 
    char* cmd = stack_alloc(char, len); 
    strcpy(cmd, temp);
    cmd[len - 1] = '\0';

    int argc = 0; 
    char** argv = 0;
    argc = ParseCommandLine(cmd, 0);
    ParseCommandLine(cmd, 0);
    argv = stack_alloc(char*, argc + 1); 
    ParseCommandLine(cmd, argv);

    int status = console_main(argc, argv); 

    return 0;
}


void wog_swapBuffers(wog_Window* wnd)
{
    if (wnd)
    {
        SwapBuffers(wnd->hdc);
    }
}


void wog_getMouse(wog_Window* wnd, int *x, int *y)
{
    POINT p;
    GetCursorPos(&p); 
    ScreenToClient(wnd->hwnd, &p);
    int w, h; 
    wog_getwindowsize(wnd, &w, &h); 
    *x = clamp(p.x, 0, w); 
    *y = clamp(p.y, 0, h);
}


void wog_quit()
{

}


void wog_getwindowsize(wog_Window* wnd, int* width, int* height)
{
    RECT r; 
    GetClientRect(wnd->hwnd, &r);
    *width = r.right;
    *height = r.bottom;
}


void wog_show(wog_Window* wnd)
{
    ShowWindow(wnd->hwnd, SW_SHOW);
}


void wog_hide(wog_Window* wnd)
{
    ShowWindow(wnd->hwnd, SW_HIDE);
}


void wog_sleep(int ms)
{
    Sleep(ms);
}