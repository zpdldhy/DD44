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
#include "ViewPortTexture.h"
#include "UIManager.h"
#include "ObjectManager.h"

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
		DXWRITE->Create();
		GUI->Init();
	}
	_app->Init();

	// Manager 초기화
	{
		CAMERAMANAGER->Init();
	}

	// ViewPort를 이용한 3DWorld Texture Rendering
	Create3DWorld();
}

void Engine::Frame()
{
	// Object Tick
	{
		OBJECTMANAGER->Tick();
		UIMANAGER->Tick();
	}

	GET_SINGLE(Device)->Frame();

	GUI->Update();
	
	_app->Update();
	if (INPUT->GetButtonDown(A))
	{
		INPUT->GetMousePos();
	}
	TIMER->Update();
	
	// Manager Tick
	{
		CAMERAMANAGER->Tick();
	}

	m_p3DWorld->Tick();
}

void Engine::Render()
{
	GET_SINGLE(Device)->PreRender();
	DXWRITE->m_pd2dRT->BeginDraw();

	D2D1_RECT_F rt = { 0.0f, 0.0f, 800.0f, 600.0f };
	DXWRITE->Draw(rt, TIMER->m_szTime);

	CAMERAMANAGER->Render(CameraViewType::CVT_ACTOR);

	_app->Render();
	// 3D World -> Texture Render
	{
		m_p3DWorldTexture->BeginViewPort();
		// ObjectList Render
		OBJECTMANAGER->Render();
		m_p3DWorldTexture->EndViewPort();

		// 3DWorld를 보여주는 평면은 Rasterizer = SolidNone으로 고정
		if (m_pCurrentRasterizer)
			m_pCurrentRasterizer.Reset();

		DC->RSGetState(m_pCurrentRasterizer.GetAddressOf());
		DC->RSSetState(STATE->m_pRSSolidNone.Get());

		{
			CAMERAMANAGER->Render(CameraViewType::CVT_UI);
			m_p3DWorld->Render();
		}

		DC->RSSetState(m_pCurrentRasterizer.Get());
		m_pCurrentRasterizer.Reset();
	}	

	UIMANAGER->Render();

	GUI->Render(); // *Fix Location* after _app->Render() 

	DXWRITE->m_pd2dRT->EndDraw();
	GET_SINGLE(Device)->PostRender();
}

void Engine::Release()
{
	OBJECTMANAGER->Destroy();
	UIMANAGER->Destroy();

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

void Engine::Create3DWorld()
{
	m_p3DWorld = make_shared<AActor>();

	auto pMesh = UStaticMeshComponent::CreatePlane();

	auto pMaterial = make_shared<UMaterial>();
	pMaterial->Load(L"", L"../Resources/Shader/Default.hlsl");
	pMesh->SetMaterial(pMaterial);

	m_p3DWorld->SetMeshComponent(pMesh);
	m_p3DWorld->SetPosition(Vec3(0.f, 0.f, 1.f));
	m_p3DWorld->SetScale(Vec3(1440.f, 900.f, 0.f));
	m_p3DWorld->Init();

	m_p3DWorldTexture = make_shared<ViewPortTexture>();
	m_p3DWorldTexture->CreateViewPortTexture(1440.f, 900.f);

	m_p3DWorld->GetMeshComponent<UStaticMeshComponent>()->GetMaterial()->SetTexture(m_p3DWorldTexture);
}