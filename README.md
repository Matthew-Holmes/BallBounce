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

