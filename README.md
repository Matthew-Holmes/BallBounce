# BallBounce

A simple windows application that displays a window with a ball bouncing from the sides

## Process

### 1. Template
The template for this project is taken from the microsoft walkthrough creating a windows application: https://learn.microsoft.com/en-us/cpp/windows/walkthrough-creating-windows-desktop-applications-cpp?view=msvc-170

#### Notes
The code includes `_In_` multiple times, this is SAL - source-code annotation language, and is used to add annotations about the code in a way a compiler can read and check. In this case indicating an input, that ought to be read only https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2012/hh916383(v=vs.110)?redirectedfrom=MSDN

The main action occurs in the following case of a switch statement, that happens when windows tells our application to paint onto the screen:
```
case WM_PAINT:
      hdc = BeginPaint(hWnd, &ps);

      // Here your application is laid out.
      // For this introduction, we just print out "Hello, Windows desktop!"
      // in the top left corner.
      TextOut(hdc,
         5, 5,
         greeting, _tcslen(greeting));
      // End application-specific layout section.

      EndPaint(hWnd, &ps);
      break;
```

### 2. Drawing

To draw to the window we use the GDI plus library, following the tutorial from https://learn.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-drawing-a-line-use

#### Notes

We don't use the `#include <stdafx.h>` line since this is for precompiled headers for speedup https://stackoverflow.com/questions/4726155/what-is-stdafx-h-used-for-in-visual-studio

The tutorial uses the namespace `using namespace Gdiplus;` so we should keep everything else prefixed, e.g. `std::` to avoid mix ups

`#pragma comment (lib,"Gdiplus.lib")` is a message to the compiler to leave a comment in the generated object file https://stackoverflow.com/questions/3484434/what-does-pragma-comment-mean

I added the necessary lines from the GDI tutorial into the first tutorials code, a few differences in the code used by each were:
* The GDI tutorial declared all the variables at the start of the class:
```
   HWND                hWnd;
   MSG                 msg;
   WNDCLASS            wndClass;
   GdiplusStartupInput gdiplusStartupInput;
   ULONG_PTR           gdiplusToken;
```
Whereas the application tutorial interspersed them with the code, defining before each variables' first usage
* The GDI tutorial uses `WNDCLASS            wndClass;` whereas the application tutorial uses `WNDCLASSEX wcex;` https://cboard.cprogramming.com/windows-programming/77003-whats-difference-between-wndclass-wndclassex.html
