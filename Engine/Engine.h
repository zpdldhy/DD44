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
	shared_ptr<class AActor> m_p3DWorld = nullptr;
	ComPtr<ID3D11RasterizerState> m_pCurrentRasterizer = nullptr;

	shared_ptr<class ViewPortTexture> world;
	shared_ptr<class ViewPortTexture> blur;
	shared_ptr<class ViewPortTexture> bloom;

public:
	Engine(HINSTANCE _hInstance, shared_ptr<IExecute> _app);

public:
	void Init();
	void Frame();
	void Render();
	void Release();
	void Run();

private:
	void Create3DWorld();
};

