#include "pch.h"
#include "Window.h"
#include "Device.h"
#include "ImGuiCore.h"

// EXTERN
POINT g_windowSize;
HWND g_hWnd;

// ImGUI extern
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)
{
	switch (_message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_MOUSEHOVER:
		EnableWindow(g_hWnd, true);
		EnableWindow(GUI->GetImGuiWindowHandle(), false);
		SetForegroundWindow(g_hWnd);
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
	//wcex.hCursor = LoadCursorFromFile(L"../Resources/Texture/UI/Empty.cur");
	RegisterClassExW(&wcex);
	return true;
}

bool Window::SetWindow(float _windowX, float _windowY)
{
	m_fWidth = _windowX;
	m_fHeight = _windowY;
	g_windowSize.x = static_cast<LONG>(m_fWidth);
	g_windowSize.y = static_cast<LONG>(m_fHeight);

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

	return true;
}

bool Window::SetWindowFullScreen()
{
	// 현재 모니터 정보 가져오기
	RECT desktopRect;
	GetWindowRect(GetDesktopWindow(), &desktopRect);  // 모니터 해상도 얻기

	m_fWidth = static_cast<float>(desktopRect.right);
	m_fHeight = static_cast<float>(desktopRect.bottom);
	g_windowSize.x = desktopRect.right;
	g_windowSize.y = desktopRect.bottom;

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

