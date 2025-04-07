#pragma once
#include "Window.h"
class IExecute;

class Engine
{
	Window _window;
	HINSTANCE _hInstance;
	shared_ptr<IExecute> _app = nullptr;
public:
	void Init();
	void Frame();
	void Render();
	void Release();
	void Run();
public:
	Engine(HINSTANCE _hInstance, shared_ptr<IExecute> _app);
};

