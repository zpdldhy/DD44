#pragma once
#include "Window.h"
#include "ViewPortTexture.h"
class IExecute;

class Engine
{
	Window _window;
	HINSTANCE _hInstance;
	shared_ptr<IExecute> _app = nullptr;

	// 3D World View Æò¸é
	ComPtr<ID3D11RasterizerState> m_pCurrentRasterizer = nullptr;

public:
	Engine(HINSTANCE _hInstance, shared_ptr<IExecute> _app);

public:
	void Init();
	void Frame();
	void Render();
	void Release();
	void Run();
};

