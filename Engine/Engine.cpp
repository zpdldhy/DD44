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
#include "AWindQuadActor.h"

bool g_bRangeVisibleMode;

void Engine::Init()
{
	GET_SINGLE(Device)->Init();

	// 기본값 설정 ( shader, inputlayout, linearSampler )
	{
		shared_ptr<Shader> shader = SHADER->Get(L"../Resources/Shader/Default.hlsl");
		INPUTLAYOUT->Init(shader->m_pCode);
		shared_ptr<Shader> shader2 = SHADER->Get(L"../Resources/Shader/skinningShader.hlsl");
		INPUTLAYOUT->CreateIW(shader2->m_pCode);
	}

	// 기타 기능 객체 초기화 ( input, )
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

	// Manager 초기화
	{
		OBJECT->Init();
		CAMERA->Init();
		COLLITION->Init();
		WIND->Init();
	}

	// ViewPort를 이용한 3DWorld Texture Rendering
	POSTPROCESS->Init(8);
	// 8개의 MRT DxState 초기화
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
		PARTICLE->Tick();
		WIND->Tick();
	}

	GET_SINGLE(Device)->Frame();

	_app->Tick();

	TIMER->Update();

	// Manager Tick
	{
		CAMERA->Tick();
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
		WIND->Render();                      // [1]
		WIND->CompositeQuadToTexture();     // [2]
		POSTPROCESS->SetSRVToSlot(7, WIND->GetCompositeSRV()); // [3]
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
		ImGui_ImplDX11_Shutdown();  // DX11 관련 리소스 해제
		ImGui_ImplWin32_Shutdown(); // Win32 윈도우 관련 연결 해제
		ImGui::DestroyContext();    // ImGui 컨텍스트 자체 해제
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