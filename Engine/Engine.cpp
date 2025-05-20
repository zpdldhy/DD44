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
#include "AActor.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "UIManager.h"
#include "ObjectManager.h"
#include "PostProcessManager.h"
#include "CollisionManager.h"
#include "LightManager.h"

void Engine::Init()
{
	GET_SINGLE(Device)->Init();

	// �⺻�� ���� ( shader, inputlayout, linearSampler )
	{
		shared_ptr<Shader> shader = SHADER->Get(L"../Resources/Shader/Default.hlsl");
		INPUTLAYOUT->Init(shader->m_pCode);
		shared_ptr<Shader> shader2 = SHADER->Get(L"../Resources/Shader/skinningShader.hlsl");
		INPUTLAYOUT->CreateIW(shader2->m_pCode);
	}

	// ��Ÿ ��� ��ü �ʱ�ȭ ( input, )
	{
		INPUT->Init();
		//DXWRITE->Create();
		GUI->Init();
	}
	_app->Init();

	// Manager �ʱ�ȭ
	{
		CAMERA->Init();
		COLLITION->Init();
	}

	// ViewPort�� �̿��� 3DWorld Texture Rendering
	POSTPROCESS->Init(8);
	// 8���� MRT DxState �ʱ�ȭ
	STATE->Create();
}

void Engine::Frame()
{
	INPUT->Tick();

	// Object Tick
	{
		OBJECT->Tick();
		LIGHTMANAGER->UpdateLightCB();

		UI->Tick();
	}

	GET_SINGLE(Device)->Frame();

	GUI->Update();
	
	_app->Update();

	TIMER->Update();
	
	// Manager Tick
	{
		CAMERA->Tick();
	}
}

void Engine::Render()
{
	GET_SINGLE(Device)->PreRender();
	//DXWRITE->m_pd2dRT->BeginDraw();

	D2D1_RECT_F rt = { 0.0f, 0.0f, 800.0f, 600.0f };
	//DXWRITE->Draw(rt, TIMER->m_szTime);

	CAMERA->Render(CameraViewType::CVT_ACTOR);

	_app->Render();
	// 3D World -> Texture Render
	{
		POSTPROCESS->PreRender();		
		OBJECT->Render();	// ObjectList Render
		POSTPROCESS->PostRender();
		
		if (m_pCurrentRasterizer)
			m_pCurrentRasterizer.Reset();

		DC->RSGetState(m_pCurrentRasterizer.GetAddressOf());
		DC->RSSetState(STATE->m_pRSSolidNone.Get());

		{
			CAMERA->Render(CameraViewType::CVT_UI);		
			POSTPROCESS->Present();
		}

		UI->Render();

		DC->RSSetState(m_pCurrentRasterizer.Get());
		m_pCurrentRasterizer.Reset();
	}	

	GUI->Render(); // *Fix Location* after _app->Render() 

	//DXWRITE->m_pd2dRT->EndDraw();
	GET_SINGLE(Device)->PostRender();
}

void Engine::Release()
{
	UI->Destroy();	

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
