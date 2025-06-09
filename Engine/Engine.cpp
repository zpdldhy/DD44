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
#include "ParticleManager.h"
#include "RenderStateManager.h"
#include "EffectManager.h"
#include "Sound.h"
#include "WindManager.h"

bool g_bRangeVisibleMode;

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
		TIMER->Init();
		LIGHTMANAGER->Init();
		INPUT->Init();
		DXWRITE->Create();
		SOUNDMANAGER->LoadAllSounds();

		if (_app->m_type != SCENE_TYPE::GAME)
		{
			GUI->Init();
			g_bRangeVisibleMode = true;
		}
		else
			g_bRangeVisibleMode = false;
	}
	_app->Init();

	// Manager �ʱ�ȭ
	{
		OBJECT->Init();
		CAMERA->Init();
		COLLITION->Init();
		WIND->Init();
	}

	// ViewPort�� �̿��� 3DWorld Texture Rendering
	POSTPROCESS->Init(8);
	// 8���� MRT DxState �ʱ�ȭ
	STATE->Create();
}

void Engine::Frame()
{
	INPUT->Tick();
	TIMER->Update();

	if(INPUT->GetButton(GameKey::ESC))
		m_bGamePaused = !m_bGamePaused;
	
	// Object Tick
	{
		if (!m_bGamePaused)
			OBJECT->ObjectMove();			// 1. Move
		LIGHTMANAGER->UpdateLightCB();

		UI->Tick();
		if (!m_bGamePaused)
			PARTICLE->Tick();
	}
	_app->Tick();						// 2. �浹

	if (!m_bGamePaused)
	{
		OBJECT->CollisionStabilization();	// 3. ����
		OBJECT->Tick();
	}

	// Manager Tick
	{
		CAMERA->Tick();
		WIND->Tick();
	}

	if (_app->m_type != SCENE_TYPE::GAME)
	{
		if (INPUT->GetButton(V))
			Device::GetInstance()->ChangeWireFrame();
		GUI->Update();
	}
}

void Engine::Render()
{
	GET_SINGLE(Device)->PreRender();
	DXWRITE->BeginDraw();

#ifndef DEBUG
	D2D1_RECT_F rt = { 0.0f, 0.0f, 800.0f, 600.0f };
	DXWRITE->Draw(rt, TIMER->m_szTime);
#endif // DEBUG

	CAMERA->Render(CameraViewType::CVT_ACTOR);

	_app->Render();
	// 3D World -> Texture Render
	{
		POSTPROCESS->PreRender();
		OBJECT->Render();	// ObjectList Render
		PARTICLE->Render();
		WIND->Render();
		POSTPROCESS->SetSRVToSlot(7, WIND->GetSRV());
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

	DXWRITE->EndDraw();
	GET_SINGLE(Device)->PostRender();

	if (_app->m_type != SCENE_TYPE::GAME)
		GUI->Render();
}

void Engine::Release()
{
	UI->Destroy();	
	PARTICLE->Destroy();
	_app->Destroy();

	if (_app->m_type != SCENE_TYPE::GAME)
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
	_window.SetWindowFullScreen();

	if (_app->m_type != SCENE_TYPE::GAME)
		GUI->CreateImGuiWindow(_hInstance);

	g_bRangeVisibleMode = true;

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