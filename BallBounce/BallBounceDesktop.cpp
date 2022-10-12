#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include <objidl.h>
#include <gdiplus.h>

#include <random>
#include <cmath>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
// Pragma comment is a message to the compiler to inclue a comment in the generated object file


// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
VOID OnPaint(HDC, REAL, REAL, int, int, int, int, int);


// Global variables for windows
static TCHAR szWindowClass[] = _T("Ball Bounce");
static TCHAR szTitle[] = _T("Ball Bounce");
HINSTANCE hInst;

// Globals for ball
constexpr REAL RADIUS = 50.0f;
constexpr REAL XSPEED = 1.0f;
constexpr REAL YSPEED = 0.75f;
const REAL SPEED = std::sqrt(XSPEED * XSPEED + YSPEED * YSPEED); 
REAL xPos = 100.0f, yPos = 100.0f, xVel = XSPEED, yVel = YSPEED;
// for adding noise to bounce
std::default_random_engine GENERATOR;
std::uniform_real_distribution<REAL> DISTRIBUTION(-0.1f, 0.1f);
// timer toggle
int IDTIMER = 1;

VOID OnPaint(HDC hdc, REAL x, REAL y, int r, int g, int b, int width = 0, int height = 0)
// paint a ball at (x,y) with colour (r, g, b) onto window with width and height
{
    Graphics graphics(hdc);
    Pen      ColorPen(Color(r, g, b));
    SolidBrush ColorSolidBrush(Color(r, g, b));
    SolidBrush back(Color(255, 255, 255));

    // background
    graphics.FillRectangle(&back, 0, 0, width, height);
    // Create a Rect object that bounds the ellipse.
    RectF ellipseRect(x - RADIUS, y - RADIUS ,2 * RADIUS, 2 *RADIUS);
    // Draw the ellipse.
    graphics.DrawEllipse(&ColorPen, ellipseRect);
    graphics.FillEllipse(&ColorSolidBrush, ellipseRect);
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
        SetTimer(hWnd, IDTIMER = 1, 10, NULL);
        break;
    case WM_TIMER: {
        InvalidateRect(hWnd, NULL, TRUE);
        xPos += xVel;
        yPos += yVel;
        GetClientRect(hWnd, &rc);

        if (xPos - RADIUS <= 0 || xPos + RADIUS >= rc.right) {
            // ensure no shivering on margin
            double eps = DISTRIBUTION(GENERATOR);
            xVel = ((1 - 2 * (xPos - RADIUS > 0)) * XSPEED) + eps;
            yVel = (1 - 2 * (yVel < 0)) * std::sqrt(SPEED * SPEED - xVel * xVel);
            // maintain constant speed
        }
        if (yPos - RADIUS <= 0 || yPos + RADIUS >= rc.bottom) {
            double eps = DISTRIBUTION(GENERATOR);
            yVel = ((1 - 2 * (yPos - RADIUS > 0)) * YSPEED) + eps;
            xVel = (1 - 2 * (xVel < 0)) * std::sqrt(SPEED * SPEED - yVel * yVel);
        }
    }
        break;
    case WM_PAINT:
        GetClientRect(hWnd, &rc);
        hdc = BeginPaint(hWnd, &ps);
        // construct a virtual DC to paint onto
        hdcBuffer = CreateCompatibleDC(hdc);
        bufBM = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        SelectObject(hdcBuffer, bufBM);
        // paint onto it
        OnPaint(hdcBuffer, xPos, yPos, 255, 0, 0, rc.right, rc.bottom);
        // copy into the actual window
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
    case WM_SIZE:
        switch (wParam)
        {
        case SIZE_MINIMIZED:
            // Stop the timer if the window is minimized. 
            KillTimer(hWnd, 1);
            IDTIMER = -1;
            break;
        case SIZE_RESTORED:
            if (IDTIMER == -1)
                SetTimer(hWnd, IDTIMER = 1, 10, NULL);
            break;
        case SIZE_MAXIMIZED:
            if (IDTIMER == -1)
                SetTimer(hWnd, IDTIMER = 1, 10, NULL);
            break;
        }
        return 0L;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}