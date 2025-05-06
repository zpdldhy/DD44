#pragma once
#include "Window.h"
class IExecute;

class Engine
{
	Window _window;
	HINSTANCE _hInstance;
	shared_ptr<IExecute> _app = nullptr;

	// 3D World View
	shared_ptr<class AActor> m_p3DWorld = nullptr;
	shared_ptr<class ViewPortTexture> m_p3DWorldTexture = nullptr;
	ComPtr<ID3D11RasterizerState> m_pCurrentRasterizer = nullptr;

public:
	void Init();
	void Frame();
	void Render();
	void Release();
	void Run();
public:
	Engine(HINSTANCE _hInstance, shared_ptr<IExecute> _app);
private:
	void Create3DWorld();
	void Render3DWorld();
};

