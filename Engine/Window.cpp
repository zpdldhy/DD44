#include "pch.h"
#include "Window.h"
#include "Device.h"

// EXTERN
POINT g_windowSize;
HWND g_hWnd;

// ImGUI extern
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(_hWnd, _message, _wParam, _lParam))
    {
        return true;
    }

    switch (_message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    //case WM_SIZE:
    //    if (_wParam != SIZE_MINIMIZED)
    //    {
    //        UINT width = LOWORD(_lParam);
    //        UINT height = HIWORD(_lParam);

    //        GET_SINGLE(Device)->OnResize(width, height);
    //    }
        return 0;
    }
    return  DefWindowProc(_hWnd, _message, _wParam, _lParam);
}

MONITORINFOEX GetSecondMonitorInfo()
{
    std::vector<MONITORINFOEX> monitors;
    EnumDisplayMonitors(nullptr, nullptr,
        [](HMONITOR hMon, HDC, LPRECT, LPARAM data) -> BOOL {
            auto& out = *reinterpret_cast<std::vector<MONITORINFOEX>*>(data);
            MONITORINFOEX mi = {};
            mi.cbSize = sizeof(mi);
            GetMonitorInfo(hMon, &mi);
            out.push_back(mi);
            return TRUE;
        }, reinterpret_cast<LPARAM>(&monitors));

    if (monitors.size() >= 2)
        return monitors[1];  // 두 번째 모니터
    else
        return monitors[0];  // 모니터가 하나뿐이면 첫 번째 사용
}

bool Window::SetWindowClass(HINSTANCE _hInstance)
{
	m_hInstance = _hInstance;
    WNDCLASSEXW wcex;
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = m_hInstance;
    wcex.lpszClassName = L"DD44";
    RegisterClassExW(&wcex);
    return true;
}

bool Window::SetWindow(float _windowX, float _windowY)
{
    m_fWidth = _windowX;
    m_fHeight = _windowY;
    g_windowSize.x = m_fWidth;
    g_windowSize.y = m_fHeight;

    RECT winRect = { 0, 0, g_windowSize.x,  g_windowSize.y };
    AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);

    m_hWnd = CreateWindow(
            L"DD44", L"DD44", WS_OVERLAPPEDWINDOW, 0, 0, 
            winRect.right - winRect.left, winRect.bottom - winRect.top, nullptr, nullptr, m_hInstance, nullptr);

    if (!m_hWnd)
    {
        return false;
    }

    g_hWnd = m_hWnd;

    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    MONITORINFOEX secondMonitor = GetSecondMonitorInfo();
    RECT rc = secondMonitor.rcMonitor;
    int x = rc.left;
    int y = rc.top;

    m_hWndImGui = CreateWindow(
        L"DD44", L"Tool", WS_OVERLAPPEDWINDOW,
        x, y, 800, m_fHeight, nullptr, nullptr, m_hInstance, nullptr);

    if (!m_hWndImGui)
    {
        return false;
    }

    return true;
}

bool Window::SetWindowFullScreen()
{
    // 현재 모니터 정보 가져오기
    RECT desktopRect;
    GetWindowRect(GetDesktopWindow(), &desktopRect);  // 모니터 해상도 얻기

    m_fWidth = desktopRect.right;
    m_fHeight = desktopRect.bottom;
    g_windowSize.x = m_fWidth;
    g_windowSize.y = m_fHeight;

    RECT winRect = { 0, 0, g_windowSize.x,  g_windowSize.y };
    m_hWnd = CreateWindow(
        L"DD44", L"DD44", WS_OVERLAPPEDWINDOW, 0, 0,
        winRect.right - winRect.left, winRect.bottom - winRect.top, nullptr, nullptr, m_hInstance, nullptr);

    if (!m_hWnd)
    {
        return false;
    }

    g_hWnd = m_hWnd;

    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    // 윈도우 스타일 변경: 타이틀바/테두리 제거
    SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);

    // 윈도우 위치 및 크기 조정: 모니터 전체
    SetWindowPos(m_hWnd, HWND_TOP,
        desktopRect.left, desktopRect.top,
        desktopRect.right - desktopRect.left,
        desktopRect.bottom - desktopRect.top,
        SWP_FRAMECHANGED | SWP_NOOWNERZORDER);

    MONITORINFOEX secondMonitor = GetSecondMonitorInfo();
    RECT rc = secondMonitor.rcMonitor;
    int x = rc.left;
    int y = rc.top;

    m_hWndImGui = CreateWindow(
        L"DD44", L"Tool", WS_OVERLAPPEDWINDOW,
        x, y, 800, m_fHeight, nullptr, nullptr, m_hInstance, nullptr);

    if (!m_hWndImGui)
    {
        return false;
    }

    ShowWindow(m_hWndImGui, SW_SHOW);
    UpdateWindow(m_hWndImGui);

    return true;
}

bool Window::ProcessMessage()
{
    MSG msg;

    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            m_bRun = false;
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        return true;
    }
    return false;
}

bool Window::CheckRun()
{
    return m_bRun;
}

