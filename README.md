# WinDarkTitlebarImpl
Handles dark title bar toggling using SetWindowCompositionAttribute on supported Windows builds, with fallback to the legacy UseImmersiveDarkModeColors property for pre-1903 versions..

```cpp
WinDarkTitlebarImpl winDarkImpl;
winDarkImpl.init();
HWND hWnd = /* dialog or main window */;
winDarkImpl.setTitleBarTheme(hWnd, true); // true=>dark, false=>light
```

You can copy and include `WinDarkTitlebarImpl.h` directly into your project. 
This file is completely free to use, modify, and distribute without any restrictions or requirements.
