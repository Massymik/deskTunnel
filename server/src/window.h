#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <windows.h>
#include <shellapi.h>
#include <thread>
#include <atomic>

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_SHOW 1001
#define ID_TRAY_HIDE 1002
#define ID_TRAY_EXIT 1003

inline HWND g_hiddenWindow = nullptr;
inline HWND g_consoleHwnd = nullptr;
inline HWND g_trayHwnd = nullptr;
inline NOTIFYICONDATAW g_nid{};
inline std::thread g_trayThread;
inline std::atomic<bool> g_trayReady = false;
inline std::atomic<bool> g_trayIconVisible = false;
inline UINT g_taskbarCreatedMsg = 0;
inline void AddOrUpdateTrayIcon();
inline void RemoveTrayIcon();
inline void ShowConsole();
inline void HideToTray();

inline void ShowConsole() {
  HWND hwnd = g_hiddenWindow;
  if (!hwnd) {
    hwnd = g_consoleHwnd;
  }
  if (!hwnd) {
    hwnd = GetConsoleWindow();
  }
  if (hwnd) {
    ShowWindowAsync(hwnd, SW_SHOW);
    ShowWindowAsync(hwnd, SW_RESTORE);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    SetForegroundWindow(hwnd);
    BringWindowToTop(hwnd);
  }
}

inline void RemoveTrayIcon() {
  if (g_trayIconVisible) {
    Shell_NotifyIconW(NIM_DELETE, &g_nid);
    g_trayIconVisible = false;
  }
}

inline void AddOrUpdateTrayIcon() {
  if (!g_trayHwnd) return;
  ZeroMemory(&g_nid, sizeof(g_nid));
  g_nid.cbSize = sizeof(NOTIFYICONDATAW);
  g_nid.hWnd = g_trayHwnd;
  g_nid.uID = 1;
  g_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
  g_nid.uCallbackMessage = WM_TRAYICON;
  g_nid.hIcon = LoadIconW(nullptr, MAKEINTRESOURCEW(32512));
  wcscpy_s(g_nid.szTip, L"RemoteAccessTool działa w tle");

  if (!g_trayIconVisible) {
    Shell_NotifyIconW(NIM_ADD, &g_nid);
    g_trayIconVisible = true;
    g_nid.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &g_nid);
  } else {
    Shell_NotifyIconW(NIM_MODIFY, &g_nid);
  }
}

inline LRESULT CALLBACK TrayWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (msg == g_taskbarCreatedMsg) {
    if (g_trayIconVisible) {
      g_trayIconVisible = false;
      AddOrUpdateTrayIcon();
    }

    return 0;
  }

  switch (msg) {
    case WM_TRAYICON:
      if (LOWORD(lParam) == WM_RBUTTONUP) {
        POINT pt;
        GetCursorPos(&pt);
        HMENU menu = CreatePopupMenu();
        AppendMenuW(menu, MF_STRING, ID_TRAY_SHOW, L"Show Console");
        AppendMenuW(menu, MF_STRING, ID_TRAY_HIDE, L"Hide Console");
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(menu, MF_STRING, ID_TRAY_EXIT, L"Exit");
        SetForegroundWindow(hwnd);
        int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);
        DestroyMenu(menu);
        if (cmd == ID_TRAY_SHOW) {
          ShowConsole();
        } else if (cmd == ID_TRAY_HIDE) {
          HideToTray();
        } else if (cmd == ID_TRAY_EXIT) {
          RemoveTrayIcon();
          ExitProcess(0);
        }

        return 0;
      }
      if (LOWORD(lParam) == WM_LBUTTONDBLCLK) {
        ShowConsole();
        return 0;
      }
      break;
    case WM_DESTROY:
      RemoveTrayIcon();
      PostQuitMessage(0);
      return 0;
  }

  return DefWindowProcW(hwnd, msg, wParam, lParam);
}

inline void TrayThreadMain() {
  g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
  WNDCLASSEXW wc{};
  wc.cbSize = sizeof(WNDCLASSEXW);
  wc.lpfnWndProc = TrayWindowProc;
  wc.hInstance = GetModuleHandleW(nullptr);
  wc.lpszClassName = L"RemoteAccessToolTrayClass";
  RegisterClassExW(&wc);
  g_trayHwnd = CreateWindowExW(0, wc.lpszClassName, L"RemoteAccessToolTrayWindow", WS_OVERLAPPED, 0, 0, 0, 0, nullptr,
                               nullptr, wc.hInstance, nullptr);
  g_trayReady = true;

  MSG msg;
  while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }
}

inline void InitializeTray() {
  g_consoleHwnd = GetConsoleWindow();
  if (g_trayReady) return;
  g_trayThread = std::thread(TrayThreadMain);
  g_trayThread.detach();
  while (!g_trayReady) {
    Sleep(10);
  }
}

inline void HideToTray() {
  if (!g_consoleHwnd) {
    g_consoleHwnd = GetConsoleWindow();
  }
  AddOrUpdateTrayIcon();
  if (g_consoleHwnd) {
    HWND hwnd = g_consoleHwnd;
    HWND root = GetAncestor(hwnd, GA_ROOTOWNER);
    if (root) {
      hwnd = root;
    }
    g_hiddenWindow = hwnd;
    ShowWindowAsync(hwnd, SW_HIDE);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
  }
}

inline void ShutdownTray() {
  RemoveTrayIcon();
  if (g_trayHwnd) {
    PostMessageW(g_trayHwnd, WM_CLOSE, 0, 0);
  }
}