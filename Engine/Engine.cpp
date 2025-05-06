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
	Create3DWorld();
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
	m_p3DWorld->Tick();
}

void Engine::Render()
{
	GET_SINGLE(Device)->PreRender();
	DXWRITE->m_pd2dRT->BeginDraw();

	D2D1_RECT_F rt = { 0.0f, 0.0f, 800.0f, 600.0f };
	DXWRITE->Draw(rt, TIMER->m_szTime);

	D2D1_RECT_F rect = D2D1::RectF(50, 50, 400, 200);
	/*std::wstring text = L"�̰��� ��Ƽ���� �ؽ�Ʈ�Դϴ�.\n�ڵ� �ٹٲ޵� �ǰ�, ������ ����� �ʾƿ�!\n�۾� ũ�⸦ �ٲٸ� �ڵ����� �������ϴ�.";
	DxWrite::GetInstance()->DrawMultiline(rect, text);*/
	CAMERAMANAGER->Render(CameraViewType::CVT_ACTOR);

	// 3D World -> Texture Render
	{
		Render3DWorld();
	}

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

void Engine::Create3DWorld()
{
	m_p3DWorld = make_shared<AActor>();

	auto pMesh = UStaticMeshComponent::CreatePlane();

	auto pMaterial = make_shared<UMaterial>();
	pMaterial->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
	pMesh->SetMaterial(pMaterial);

	m_p3DWorld->SetMeshComponent(pMesh);
	m_p3DWorld->SetPosition(Vec3(0.f, 0.f, 1.f));
	m_p3DWorld->SetScale(Vec3(1440.f, 900.f, 0.f));
	m_p3DWorld->Init();

	m_p3DWorldTexture = make_shared<ViewPortTexture>();
	m_p3DWorldTexture->CreateViewPortTexture(1440.f, 900.f);

	m_p3DWorld->GetMeshComponent<UStaticMeshComponent>()->GetMaterial()->GetTexture()->SetSRV(m_p3DWorldTexture->GetSRV());
}

void Engine::Render3DWorld()
{
	m_p3DWorldTexture->BeginViewPort();
	_app->Render();
	m_p3DWorldTexture->EndViewPort();

	if (m_pCurrentRasterizer)
		m_pCurrentRasterizer.Reset();

	DC->RSGetState(m_pCurrentRasterizer.GetAddressOf());
	DC->RSSetState(STATE->m_pRSSolidNone.Get());

	CAMERAMANAGER->Render(CameraViewType::CVT_UI);
	m_p3DWorld->Render();

	DC->RSSetState(m_pCurrentRasterizer.Get());
	m_pCurrentRasterizer.Reset();
}
