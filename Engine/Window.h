#pragma once

class Window
{
	HINSTANCE m_hInstance = HINSTANCE();
	HWND m_hWnd = HWND();

	float m_fWidth = 0.f;
	float m_fHeight = 0.f;
	bool m_bRun = true;

public:
	bool SetWindowClass(HINSTANCE _hInstance);
	bool SetWindow(float _windowX = 1440.0f, float _windowY = 900.0f);
	bool SetWindowFullScreen();
	bool ProcessMessage();
public:
	bool CheckRun();
};

