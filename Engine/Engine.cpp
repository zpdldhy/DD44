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
	POSTPROCESS->Init(static_cast<UINT>(g_windowSize.x), static_cast<UINT>(g_windowSize.y));

	world.CreateViewPortTexture(1440.f, 900.f);
	blur.CreateViewPortTexture(1440.f, 900.f);
	bloom.CreateViewPortTexture(1440.f, 900.f);
	m_p3DWorld->GetMeshComponent<UStaticMeshComponent>()->GetMaterial()->SetTexture(world.GetSRV());
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
		vector<ID3D11RenderTargetView*> RTVList = { world.GetRTV(), blur.GetRTV() , bloom.GetRTV() };
		vector<D3D11_VIEWPORT> VPList = { world.GetVP(), blur.GetVP() , bloom.GetVP() };

		POSTPROCESS->PreRender(2, RTVList, world.GetDSV(), VPList);
		// ObjectList Render
		OBJECTMANAGER->Render();
		POSTPROCESS->PostRender();

		//POSTPROCESS->Blur(m_p3DWorldTexture->GetSRV());                    // 2-pass Blur
		//POSTPROCESS->RenderCombine(m_p3DWorldTexture->GetSRV());           // Combine 결과를 백버퍼에 출력

		//m_p3DWorld->GetMeshComponent<UStaticMeshComponent>()->GetMaterial()->SetTexture(POSTPROCESS->GetBlurResultSRV().Get());

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
	float fWinSizeX = static_cast<float>(g_windowSize.x);
	float fWinSizeY = static_cast<float>(g_windowSize.y);

	m_p3DWorld = make_shared<AActor>();

	auto pMesh = UStaticMeshComponent::CreatePlane();

	auto pMaterial = make_shared<UMaterial>();
	pMaterial->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/PRDefault.hlsl");
	pMesh->SetMaterial(pMaterial);

	m_p3DWorld->SetMeshComponent(pMesh);
	m_p3DWorld->SetPosition(Vec3(0.f, 0.f, 1.f));
	m_p3DWorld->SetScale(Vec3(fWinSizeX, fWinSizeY, 0.f));
	m_p3DWorld->Init();

	m_p3DWorldTexture = make_shared<ViewPortTexture>();
	m_p3DWorldTexture->CreateViewPortTexture(fWinSizeX, fWinSizeY);

	//m_p3DWorld->GetMeshComponent<UStaticMeshComponent>()->GetMaterial()->SetTexture(m_p3DWorldTexture);
}