#include "pch.h"
#include "Engine.h"
#include "IExecute.h"
#include "Device.h"
#include "Inputlayout.h"
#include "DxState.h"
#include "Shader.h"
#include "Input.h"

void Engine::Init()
{
	GET_SINGLE(Device)->Init();

	// 기본값 설정 ( shader, inputlayout, linearSampler )
	{
		shared_ptr<Shader> shader = SHADER->Get(L"../Resources/Shader/Default.hlsl");
		INPUTLAYOUT->Init(shader->m_pCode);
		STATE->Create();
	}

	// 기타 기능 객체 초기화 ( input, )
	{
		INPUT->Init();
	}
	_app->Init();
}

void Engine::Frame()
{
	_app->Update();

	if (INPUT->GetButtonDown(A))
	{
		INPUT->GetMousePos();
	}
}

void Engine::Render()
{
	GET_SINGLE(Device)->PreRender();
	_app->Render();
	GET_SINGLE(Device)->PostRender();
}

void Engine::Release()
{
	GET_SINGLE(Device)->Release();
}

void Engine::Run()
{
	_window.SetWindowClass(_hInstance);
	_window.SetWindow();
	Init();
	while (_window.CheckRun())
	{
		if (!_window.ProcessMessage())
		{
			Frame();
			Render();
		}
	}
	Release();
}

Engine::Engine(HINSTANCE hInstance, shared_ptr<IExecute> app)
{
	_hInstance = hInstance;
	_app = app;
}