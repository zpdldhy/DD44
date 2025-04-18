#pragma once

class Window
{
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	float m_fWidth;
	float m_fHeight;
	bool m_bRun = true;

public:
	bool SetWindowClass(HINSTANCE _hInstance);
	bool SetWindow(float _windowX = 800.0f, float _windowY = 600.0f);
	bool ProcessMessage();
public:
	bool CheckRun();
	 
};

