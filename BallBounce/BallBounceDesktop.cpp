// HelloWindowsDesktop.cpp
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c
// https://learn.microsoft.com/en-us/cpp/windows/walkthrough-creating-windows-desktop-applications-cpp?view=msvc-170

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include <iostream>

// For drawing 
// https://learn.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-drawing-a-line-use
// The <stdafx.h> is omitted since that is for precompiled headers
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
// Pragma comment is a message to the compiler to inclue a comment in the generated object file


// Forward declarations of functions included in this code module:
// Used in both tutorials
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


// Global variables

// The main window class name.
static TCHAR szWindowClass[] = _T("Ball Bounce");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Ball Bounce");

// Stored instance handle for use in Win32 API calls such as FindResource
HINSTANCE hInst;

// Radius of the ball we draw
// use float since this is the datatype REAL used by GDIplus
constexpr REAL RADIUS = 50.0f;
constexpr REAL XSPEED = 1.0f;
constexpr REAL YSPEED = 0.75f;
REAL xPos = 100.0f, yPos = 100.0f, xVel = XSPEED, yVel = YSPEED;

VOID OnPaint(HDC hdc, REAL x, REAL y, int r, int g, int b, int width = 0, int height = 0)
// paint a ball at (x,y)
{
    Graphics graphics(hdc);
    Pen      redPen(Color(r, g, b));
    SolidBrush redSolidBrush(Color(r, g, b));
    SolidBrush back(Color(255, 255, 255));

    // background
    graphics.FillRectangle(&back, 0, 0, width, height);
    // Create a Rect object that bounds the ellipse.
    RectF ellipseRect(x - RADIUS, y - RADIUS ,2 * RADIUS, 2 *RADIUS);
    // Draw the ellipse.
    graphics.DrawEllipse(&redPen, ellipseRect);
    graphics.FillEllipse(&redSolidBrush, ellipseRect);
}


int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    WNDCLASSEX wcex;

    // GDI stuff
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Ball Bounce"),
            NULL);

        return 1;
    }

    // Store instance handle in our global variable
    hInst = hInstance;

    // The parameters to CreateWindowEx explained:
    // WS_EX_OVERLAPPEDWINDOW : An optional extended window style.
    // szWindowClass: the name of the application
    // szTitle: the text that appears in the title bar
    // WS_OVERLAPPEDWINDOW: the type of window to create
    // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
    // 500, 100: initial size (width, length)
    // NULL: the parent of this window
    // NULL: this application does not have a menu bar
    // hInstance: the first parameter from WinMain
    // NULL: not used in this application
    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        700, 400,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Ball Bounce"),
            NULL);

        return 1;
    }

    // The parameters to ShowWindow explained:
    // hWnd: the value returned from CreateWindow
    // nCmdShow: the fourth parameter from WinMain
    ShowWindow(hWnd,
        nCmdShow);
    UpdateWindow(hWnd);

    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    HDC hdcBuffer;
    RECT rc; 
    HBITMAP bufBM;


    switch (message)
    {
    case WM_CREATE:
        hdc = GetDC(hWnd);
        SetTimer(hWnd, 1, 10, NULL);
        break;
    case WM_TIMER: {
        InvalidateRect(hWnd, NULL, TRUE);
        xPos += xVel;
        yPos += yVel;
        GetClientRect(hWnd, &rc);
        if (xPos - RADIUS <= 0 || xPos + RADIUS >= rc.right) {
            // ensure no shivering on margin
            xVel = (1 - 2 * (xPos - RADIUS > 0)) * XSPEED;
        }
        if (yPos - RADIUS <= 0 || yPos + RADIUS >= rc.bottom) {
            yVel = (1 - 2 * (yPos - RADIUS > 0)) * YSPEED;
        }
        
    }
        break;
    case WM_PAINT:
        GetClientRect(hWnd, &rc);
        hdc = BeginPaint(hWnd, &ps);
        hdcBuffer = CreateCompatibleDC(hdc);
        bufBM = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        SelectObject(hdcBuffer, bufBM);
        OnPaint(hdcBuffer, xPos, yPos, 255, 0, 0, rc.right, rc.bottom);
        BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcBuffer, 0, 0, SRCCOPY);
        DeleteObject(bufBM);
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;
    case WM_ERASEBKGND:
        return true;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}