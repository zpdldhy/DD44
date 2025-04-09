#include "pch.h"
#include "Engine.h"
#include "IExecute.h"
#include "Device.h"
#include "Inputlayout.h"
#include "DxState.h"
#include "Shader.h"
#include "Input.h"
#include "DxWrite.h"

void Engine::Init()
{
	GET_SINGLE(Device)->Init();

	// ±âº»°ª ¼³Á¤ ( shader, inputlayout, linearSampler )
	{
		shared_ptr<Shader> shader = SHADER->Get(L"../Resources/Shader/Default.hlsl");
		INPUTLAYOUT->Init(shader->m_pCode);
		STATE->Create();
		DXWRITE->Create();
	}

	// ±âÅ¸ ±â´É °´Ã¼ ÃÊ±âÈ­ ( input, )
	{
		INPUT->Init();
	}
	_app->Init();
}

void Engine::Frame()
{
	_app->Update();

	if (INPUT->GetButton(A))
	{
		DXWRITE->SetFont(L"¼Ò¾ß¶ã9",20);
	}

	if (INPUT->GetButton(S))
	{
		DXWRITE->SetFont(L"±Ã¼­", 20);
	}

	if (INPUT->GetButton(D))
	{
		DXWRITE->SetFont(L"Rubik Spray Paint", 20);
	}
	
	if (INPUT->GetButton(W))
	{
		DXWRITE->SetFont(L"MapoÈ«´ëÇÁ¸®´ý", 20);
	}

	if (INPUT->GetButton(Q))
	{
		DXWRITE->IncreaseFontSize();
	}
	if (INPUT->GetButton(E))
	{
		DXWRITE->DecreaseFontSize();
	}

	if (INPUT->GetButtonDown(A))
	{
		INPUT->GetMousePos();
	}
}

void Engine::Render()
{
	GET_SINGLE(Device)->PreRender();
	DXWRITE->m_pd2dRT->BeginDraw();

	D2D1_RECT_F rt = { 500.0f, 0.0f, 800.0f, 600.0f };
	DXWRITE->Draw(rt, L"1234567890");
	_app->Render();

	DXWRITE->m_pd2dRT->EndDraw();
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