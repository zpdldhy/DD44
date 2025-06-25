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
#include "ShadowManager.h"
#include "BloomManager.h"

bool g_bRangeVisibleMode;

void Engine::Init()
{
	GET_SINGLE(Device)->Init();

	// 기본값 설정 ( shader, inputlayout, linearSampler )
	{
		shared_ptr<Shader> shader = SHADER->Get(L"../Resources/Shader/Default.hlsl");
		INPUTLAYOUT->Init(shader->m_pCode);
		shared_ptr<Shader> shader2 = SHADER->Get(L"../Resources/Shader/skinningShader.hlsl");
		INPUTLAYOUT->CreateInstanceIW(shader2->m_pCode);
	}

	// 기타 기능 객체 초기화 ( input, )
	{
		TIMER->Init();
		LIGHT->Init();
		SHADOW->Init();
		INPUT->Init();
		DXWRITE->Create();
		SOUND->LoadAllSounds();

		if (_app->m_type != SCENE_TYPE::GAME)
		{
			GUI->Init();
			g_bRangeVisibleMode = true;
		}
		else
			g_bRangeVisibleMode = false;
	}

	OBJECT->Init();

	_app->Init();

	// Manager 초기화
	{
		CAMERA->Init();
		COLLITION->Init();
		WIND->Init();
		BLOOMMANAGER->Init();
	}

	// ViewPort를 이용한 3DWorld Texture Rendering
	POSTPROCESS->Init(8);
	// 8개의 MRT DxState 초기화
	STATE->Create();
}

void Engine::Frame()
{
	// Manager Tick
	INPUT->Tick();
	TIMER->Update();
	LIGHT->Tick();

	UI->Tick();	// Fade가 되려면 여기 위치

	if (!m_bGamePaused)
	{
		OBJECT->ObjectMove();				// 1. Move
		EFFECT->EffectMove();
	}

	_app->Tick();							// 2. 충돌

	if (!m_bGamePaused)
	{		
		OBJECT->CollisionStabilization();	// 3. 보정
		OBJECT->Tick();

		{
			CAMERA->Tick();		// Fructum Range Render 때문에 Object 뒤에서 Render
			EFFECT->Tick();
			PARTICLE->Tick();
			WIND->Tick();
		}
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

	_app->Render();
	SHADOW->Render();
	CAMERA->Render(CameraViewType::CVT_ACTOR);

	// 3D World -> Texture Render
	{
		POSTPROCESS->PreRender();
		ID3D11ShaderResourceView* shadowSRV = SHADOW->GetSRV();
		DC->PSSetShaderResources(3, 1, &shadowSRV);
		auto sampler = SHADOW->GetShadowSampler();
		DC->PSSetSamplers(1, 1, sampler.GetAddressOf());
		OBJECT->Render();	// ObjectList Render
		LIGHT->Render();
		EFFECT->Render();
		PARTICLE->Render();
		WIND->Render();
		POSTPROCESS->SetSRVToSlot(7, WIND->GetSRV());
		POSTPROCESS->SetSRVToSlot(3, BLOOMMANAGER->GetDownScaleSRV());
		POSTPROCESS->SetSRVToSlot(9, BLOOMMANAGER->GetDepthSRV());            // Bloom Depth → t9
		POSTPROCESS->SetSRVToSlot(10, POSTPROCESS->GetMRT(0)->GetDepthSRV()); // Scene Depth → t10 
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
	//_window.SetWindow();
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