#pragma once
#include "Singleton.h"
#include "Window.h"
#include "ViewPortTexture.h"

#define ENGINE GET_SINGLE(Engine)

class IExecute;

class Engine : public Singleton<Engine>
{
	Window _window;
	HINSTANCE _hInstance;
	shared_ptr<IExecute> _app = nullptr;

	// 3D World View Æò¸é
	ComPtr<ID3D11RasterizerState> m_pCurrentRasterizer = nullptr;
	bool m_bGamePaused = false;

public:
	void Init();
	void Frame();
	void Render();
	void Release();
	void Run();

public:
	void SetInstance(HINSTANCE _h) { _hInstance = _h; }
	void SetApp(shared_ptr<IExecute> app) { _app = app; }
};

