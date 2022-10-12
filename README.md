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

To draw to the window we use the GDI plus library, following the tutorial from https://learn.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-drawing-a-line-use and changing the drawing of a line to a circle (special case of an ellipse) https://learn.microsoft.com/en-us/windows/win32/api/gdiplusgraphics/nf-gdiplusgraphics-graphics-drawellipse(constpen_constrectf_)

By the end of this step we drew a red ball onto the window
```
VOID OnPaint(HDC hdc)
{
    Graphics graphics(hdc);
    Pen      redPen(Color(255, 255, 0, 0));
    SolidBrush redSolidBrush(Color(255, 255, 0, 0));

    // Create a Rect object that bounds the ellipse.
    RectF ellipseRect(50.0f, 50.0f, 100.0f, 100.0f);
    // Draw the ellipse.
    graphics.DrawEllipse(&redPen, ellipseRect);
    graphics.FillEllipse(&redSolidBrush, ellipseRect);
}
```

#### Notes

Drawing a filled in circle requires both a pen and brush, https://learn.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-brushes-and-filled-shapes-about since we want a filled shape

We don't use the `#include <stdafx.h>` line since this is for precompiled headers for speedup https://stackoverflow.com/questions/4726155/what-is-stdafx-h-used-for-in-visual-studio

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

### 3. Animating I

Now we want to have the ball move across the screen, to do this we redraw it at timed intervals, following this guide: https://learn.microsoft.com/en-us/windows/win32/gdi/drawing-at-timed-intervals. Which begins a 10 milisecond timer that sends a message to redraw the ball when it finishes. The `OnPaint` functions was updated to include variable position. 

```
case WM_TIMER:
        InvalidateRect(hWnd, NULL, true); // erase the previous image

        xPos += xVel;
        yPos += yVel;
 
        break;
```

#### Notes

It is essential to include the `InvalidateRect(hWnd, NULL, true)` line, which tells windows that it must redraw the window's contents, otherwise the ball will only appear to move if we minimise and reopen the window, or resize it. Operations which trigger the window to be forcefully updated.

Calling `InvalidateRect(hWnd, NULL, true)` triggers a message to paint the window again, now with the updated coordinates.

### 4. Animating II - Buffering

As it stands the previous steps produce a window which a ball moves across, however the graphics flicker. To solve this we use buffered paint, as discussed in: https://stackoverflow.com/questions/51329024/gdi-flickering

The detailed methodology is taken from https://forums.codeguru.com/showthread.php?523273-GDI-and-double-buffering, we draw onto a virtual context, then copy that onto the actual displayed window.

```
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
```
#### Notes

To remove one source of flickering we disable the call to erase the background:
```
case WM_ERASEBKGND:
        return true;
```
And do this by hand, first drawing a white rectangle in the `OnPaint` functionm, which now takes more inputs to both change the ball's colour and specify the size of the window.

The buffering is then necessary since the `OnPaint` funtion draws the background, the ball's outline then the ball in sucession, which unless buffered, leads to flickering.

### 4. Animating III - Rebounds

A quick update to the timer handler makes the ball bounce from the windows edge: 
```
   case WM_TIMER: {
        InvalidateRect(hWnd, NULL, TRUE);
        xPos += xVel;
        yPos += yVel;
        GetClientRect(hWnd, &rc);

        if (xPos - RADIUS <= 0 || xPos + RADIUS >= rc.right) {
            // ensure no shivering on margin
            double eps = distribution(generator);
            xVel = ((1 - 2 * (xPos - RADIUS > 0)) * XSPEED) + eps;
            yVel = (1 - 2 * (yVel < 0)) * std::sqrt(SPEED * SPEED - xVel * xVel);
            // maintain constant speed
        }
        if (yPos - RADIUS <= 0 || yPos + RADIUS >= rc.bottom) {
            double eps = distribution(generator);
            yVel = ((1 - 2 * (yPos - RADIUS > 0)) * YSPEED) + eps;
            xVel = (1 - 2 * (xVel < 0)) * std::sqrt(SPEED * SPEED - yVel * yVel);
        }
        
    }
        break;
```

#### Notes
We don't just flip the speed since this can lead to the ball getting stuck on the edge, instead setting its velocity direction according to which side of the screen the ball is at. This also means that should the ball be outside of the visible area it will move back to the visible portion.

We also include some random noise to stop the path being entirely deterministic, the noise follows distribution:
```
std::default_random_engine generator;
std::uniform_real_distribution<REAL> distribution(-0.25f, 0.25f);
```
taken from the `<random>` header

A constant `SPEED` is used to adjust the perpendicular speed also so that the ball maintains a fixed speed across the screen.

### 5. Pausing

Finally we copy the original drawing at timed interval guide and pause the motion when the window is minimise, by keeping track of an `idTimer` variable, and handling a few more messages: 

```case WM_SIZE:
        switch (wParam)
        {
        case SIZE_MINIMIZED:
            // Stop the timer if the window is minimized. 
            KillTimer(hWnd, 1);
            idTimer = -1;
            break;
        case SIZE_RESTORED:
            if (idTimer == -1)
                SetTimer(hWnd, idTimer = 1, 10, NULL);
            break;
        case SIZE_MAXIMIZED:
            if (idTimer == -1)
                SetTimer(hWnd, idTimer = 1, 10, NULL);
            break;
        }
        return 0L;
  ```

TODO - https://stackoverflow.com/questions/62173013/gdi-text-flickering-and-now-with-black-background
