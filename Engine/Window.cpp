#include "pch.h"
#include "Window.h"

// EXTERN
POINT g_windowSize;
HWND g_hWnd;

LRESULT CALLBACK WndProc(HWND m_hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return  DefWindowProc(m_hWnd, message, wParam, lParam);

}

bool Window::SetWindowClass(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
    WNDCLASSEXW wcex;
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = m_hInstance;
    wcex.lpszClassName = L"Game";
    RegisterClassExW(&wcex);
    return true;
}

bool Window::SetWindow(float windowX, float windowY)
{
    m_fWidth = windowX;
    m_fHeight = windowY;
    g_windowSize.x = m_fWidth;
    g_windowSize.y = m_fHeight;

    RECT winRect = { 0, 0, g_windowSize.x,  g_windowSize.y };
    AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);

    m_hWnd = CreateWindow(
            L"Game", L"Game", WS_OVERLAPPEDWINDOW, 0, 0, 
            winRect.right - winRect.left, winRect.bottom - winRect.top, nullptr, nullptr, m_hInstance, nullptr);
    if (!m_hWnd) { return false; }
    g_hWnd = m_hWnd;

    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

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

