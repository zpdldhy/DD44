#include "pch.h"
#include "Engine.h"
#include "IExecute.h"
#include "Device.h"
#include "Inputlayout.h"
#include "DxState.h"
#include "Shader.h"
#include "Input.h"
#include "DxWrite.h"
#include "Timer.h" 
#include "ImGuiCore.h"
#include "CameraManager.h"

void Engine::Init()
{
	GET_SINGLE(Device)->Init();

	// �⺻�� ���� ( shader, inputlayout, linearSampler )
	{
		shared_ptr<Shader> shader = SHADER->Get(L"../Resources/Shader/Default.hlsl");
		INPUTLAYOUT->Init(shader->m_pCode);
		STATE->Create();
	}

	// ��Ÿ ��� ��ü �ʱ�ȭ ( input, )
	{
		INPUT->Init();
		DXWRITE->Create();
		GUI->Init();
	}
	_app->Init();

	CAMERAMANAGER->Init();
}

void Engine::Frame()
{
	GET_SINGLE(Device)->Frame();

	GUI->Update();
	
	_app->Update();
	if (INPUT->GetButtonDown(A))
	{
		INPUT->GetMousePos();
	}
	TIMER->Update();
	
	CAMERAMANAGER->Tick();
}

void Engine::Render()
{
	GET_SINGLE(Device)->PreRender();
	DXWRITE->m_pd2dRT->BeginDraw();

	D2D1_RECT_F rt = {0.0f, 0.0f, 800.0f, 600.0f };
	DXWRITE->Draw(rt, TIMER->m_szTime);

	D2D1_RECT_F rect = D2D1::RectF(50, 50, 400, 200);
	/*std::wstring text = L"�̰��� ��Ƽ���� �ؽ�Ʈ�Դϴ�.\n�ڵ� �ٹٲ޵� �ǰ�, ������ ����� �ʾƿ�!\n�۾� ũ�⸦ �ٲٸ� �ڵ����� �������ϴ�.";
	DxWrite::GetInstance()->DrawMultiline(rect, text);*/
	CAMERAMANAGER->Render(CameraViewType::CVT_ACTOR);
	_app->Render();
	GUI->Render(); // *Fix Location* after _app->Render() 

	DXWRITE->m_pd2dRT->EndDraw();
	GET_SINGLE(Device)->PostRender();
}

void Engine::Release()
{
	{
		ImGui_ImplDX11_Shutdown();  // DX11 ���� ���ҽ� ����
		ImGui_ImplWin32_Shutdown(); // Win32 ������ ���� ���� ����
		ImGui::DestroyContext();    // ImGui ���ؽ�Ʈ ��ü ����
	}
	GET_SINGLE(Device)->Release();
}

void Engine::Run()
{
	_window.SetWindowClass(_hInstance);
	_window.SetWindow();
	Init();

	Timer timer; 
	timer.Init(); 

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