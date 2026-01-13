// By https://github.com/nesall (Arman S.)
#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#ifndef ASSERT
#include <cassert>
#define ASSERT assert
#endif

struct WinDarkTitlebarImpl {

  enum WINDOWCOMPOSITIONATTRIB
  {
    WCA_UNDEFINED = 0,
    WCA_NCRENDERING_ENABLED = 1,
    WCA_NCRENDERING_POLICY = 2,
    WCA_TRANSITIONS_FORCEDISABLED = 3,
    WCA_ALLOW_NCPAINT = 4,
    WCA_CAPTION_BUTTON_BOUNDS = 5,
    WCA_NONCLIENT_RTL_LAYOUT = 6,
    WCA_FORCE_ICONIC_REPRESENTATION = 7,
    WCA_EXTENDED_FRAME_BOUNDS = 8,
    WCA_HAS_ICONIC_BITMAP = 9,
    WCA_THEME_ATTRIBUTES = 10,
    WCA_NCRENDERING_EXILED = 11,
    WCA_NCADORNMENTINFO = 12,
    WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
    WCA_VIDEO_OVERLAY_ACTIVE = 14,
    WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
    WCA_DISALLOW_PEEK = 16,
    WCA_CLOAK = 17,
    WCA_CLOAKED = 18,
    WCA_ACCENT_POLICY = 19,
    WCA_FREEZE_REPRESENTATION = 20,
    WCA_EVER_UNCLOAKED = 21,
    WCA_VISUAL_OWNER = 22,
    WCA_HOLOGRAPHIC = 23,
    WCA_EXCLUDED_FROM_DDA = 24,
    WCA_PASSIVEUPDATEMODE = 25,
    WCA_USEDARKMODECOLORS = 26,
    WCA_LAST = 27
  };

  struct WINDOWCOMPOSITIONATTRIBDATA
  {
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
  };

  using fnRtlGetNtVersionNumbers = void (WINAPI *)(LPDWORD major, LPDWORD minor, LPDWORD build);
  using fnSetWindowCompositionAttribute = BOOL(WINAPI *)(HWND hWnd, WINDOWCOMPOSITIONATTRIBDATA *);

  fnSetWindowCompositionAttribute _SetWindowCompositionAttribute = nullptr;

  DWORD buildNumber_;

  WinDarkTitlebarImpl() : buildNumber_(0) {
  }

  bool checkBuildNumber(DWORD buildNumber) {
    return (buildNumber == 17763 || // 1809
      buildNumber == 18362 || // 1903
      buildNumber == 18363 || // 1909
      buildNumber == 19041 || // 2004
      buildNumber == 19042 || // 20H2
      buildNumber == 19043 || // 21H1
      buildNumber == 19044 || // 21H2
      (buildNumber > 19044 && buildNumber < 22000) || // Windows 10 any version > 21H2 
      buildNumber >= 22000);  // Windows 11 builds
  }

  void setTitleBarTheme(HWND hWnd, BOOL dark) {
    ASSERT(hWnd);
    ASSERT(::IsWindow(hWnd));
    if (_SetWindowCompositionAttribute) {
      WINDOWCOMPOSITIONATTRIBDATA data = { WCA_USEDARKMODECOLORS, &dark, sizeof(dark) };
      _SetWindowCompositionAttribute(hWnd, &data);
    } else {
      SetProp(hWnd, TEXT("UseImmersiveDarkModeColors"), reinterpret_cast<HANDLE>(static_cast<intptr_t>(dark)));
    }
  }

  void init() {
    fnRtlGetNtVersionNumbers RtlGetNtVersionNumbers = nullptr;
    if (HMODULE hNtdllModule = GetModuleHandle(TEXT("ntdll.dll"))) {
      RtlGetNtVersionNumbers = reinterpret_cast<fnRtlGetNtVersionNumbers>(GetProcAddress(hNtdllModule, "RtlGetNtVersionNumbers"));
    }
    if (RtlGetNtVersionNumbers) {
      DWORD major, minor;
      RtlGetNtVersionNumbers(&major, &minor, &buildNumber_);
      buildNumber_ &= ~0xF0000000;
      if (major == 10 && minor == 0 && checkBuildNumber(buildNumber_)) {
        if (HMODULE hUser32 = GetModuleHandle(TEXT("user32.dll"))) {
          _SetWindowCompositionAttribute = reinterpret_cast<fnSetWindowCompositionAttribute>(GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
        }
      }
    }
  }
};

#endif // WIN32
