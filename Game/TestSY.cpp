#include "pch.h"
#include "TestSY.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "APawn.h"
#include "ACameraActor.h"
#include "ASky.h"
#include "EngineCameraMoveScript.h"
#include "kkongchiMoveScript.h"
#include "UTerrainMeshComponent.h"
#include "UMeshResources.h"
#include "CameraManager.h"
#include "Input.h"
#include <random>
#include "ATerrainTileActor.h"
#include "ImGuiCore.h"
#include "DxState.h"
#include "MapEditorUI.h"
#include "UBoxComponent.h"

void TestSY::Init()
{
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 10.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());

		m_pCameraActor->Init();
		CAMERAMANAGER->SetCameraActor(m_pCameraActor);
	}

	{
		m_pActor = make_shared<APawn>();

		m_pStaticMesh = UStaticMeshComponent::CreateCube();
		m_pActor->SetMeshComponent(m_pStaticMesh);
		m_pActor->SetScale({ 5.0f, 5.0f, 5.0f });
		m_pActor->SetPosition({ 0.0f, 2.5f, 10.0f });
		m_pActor->SetRotation({ 0.0f, 0.0f, 0.0f });

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
		m_pStaticMesh->SetMaterial(material);

		auto pCameraComponent = make_shared<UCameraComponent>();
		pCameraComponent->SetPosition(Vec3(20.f, 20.f, -20.f));
		pCameraComponent->SetNear(10.f);
		pCameraComponent->SetFar(100.f);
		m_pActor->SetCameraComponent(pCameraComponent);

		auto pBoxComponent = make_shared<UBoxComponent>();
		pBoxComponent->SetScale({ 6.f, 6.f, 6.f });
		m_pActor->SetShapeComponent(pBoxComponent);

		m_pActor->AddScript(make_shared<kkongchiMoveScript>());

		m_pActor->Init();
	}

	{
		m_pSky = make_shared<ASky>();

		m_pSkyMesh = UStaticMeshComponent::CreateSphere(20, 20);
		m_pSky->SetMeshComponent(m_pSkyMesh);

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/Sky.jpg", L"../Resources/Shader/Sky.hlsl");
		m_pSkyMesh->SetMaterial(material);

		m_pSky->Init();
	}

	GUI->SetMapEditorCallback([this]()
	{
		MapEditorUI* editor = GUI->GetMapEditorUI();
		if (!editor) return;

		auto tile = std::make_shared<ATerrainTileActor>();

		tile->m_iNumCols = editor->GetNumCols();
		tile->m_iNumRows = editor->GetNumRows();
		tile->m_fCellSize = editor->GetCellSize();

		tile->CreateTerrain(editor->GetTexturePath(), editor->GetShaderPath());
		tile->SetPosition(editor->GetPosition());
		tile->SetRotation(editor->GetRotation());
		tile->SetScale(editor->GetScale());

		tile->Init();
		m_vTiles.push_back(tile);
	});

	{
		m_pPlane = make_shared<APawn>();
		
		auto pMesh = UStaticMeshComponent::CreatePlane();
		
		auto pMaterial = make_shared<UMaterial>();
		pMaterial->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/DefaultUI.hlsl");
		pMesh->SetMaterial(pMaterial);

		m_pPlane->SetMeshComponent(pMesh);
		m_pPlane->SetPosition(Vec3(0.f, 0.f, 1.f));
		m_pPlane->SetScale(Vec3(2.f, 2.f, 2.f));
		m_pPlane->Init();
	}

	CreateViewPortTexture();
}

void TestSY::Update()
{
	CAMERAMANAGER->SetCameraActor(m_pCameraActor);
	if (INPUT->GetButtonDown(O))
		CAMERAMANAGER->SetCameraActor(m_pActor);

	m_pCameraActor->Tick();
	m_pActor->Tick();
	m_pSky->Tick();
	m_pPlane->Tick();

	for (auto& tile : m_vTiles)
		tile->Tick();
}

void TestSY::Render()
{	
	BeginViewPort();

	if (m_bEditorWireframe)
	{
		DC->RSSetState(STATE->m_pRSWireFrame.Get());
	}
	else
	{
		DC->RSSetState(STATE->m_pRSSolid.Get());
	}

	if (INPUT->GetButtonDown(P))
		CAMERAMANAGER->Render(CameraViewType::CVT_UI);
	
	m_pCameraActor->Render();
	m_pActor->Render();
	m_pSky->Render();

	for (auto& tile : m_vTiles)
		tile->Render();

	EndViewPort();

	m_pPlane->GetMeshComponent<UStaticMeshComponent>()->GetMaterial()->GetTexture()->SetSRV(m_pSRV);
	m_pPlane->Render();
}

void TestSY::Destroy()
{
}

void TestSY::CreateViewPortTexture()
{
	m_vp.TopLeftX = 0;
	m_vp.TopLeftY = 0;
	m_vp.Width = 1440;
	m_vp.Height = 900;
	m_vp.MinDepth = 0.f;
	m_vp.MaxDepth = 1.f;

	m_TexDesc.Width = 1440;
	m_TexDesc.Height = 900;
	m_TexDesc.MipLevels = 1;
	m_TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_TexDesc.SampleDesc.Count = 1;
	m_TexDesc.SampleDesc.Quality = 0;
	m_TexDesc.Usage = D3D11_USAGE_DEFAULT;
	m_TexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	m_TexDesc.CPUAccessFlags = 0;
	m_TexDesc.MiscFlags = 0;
	m_TexDesc.ArraySize = 1;

	HRESULT hr = DEVICE->CreateTexture2D(&m_TexDesc, NULL, m_pTexture.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
	}

	hr = DEVICE->CreateShaderResourceView(m_pTexture.Get(), NULL, m_pSRV.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
	}

	hr = DEVICE->CreateRenderTargetView(m_pTexture.Get(), NULL, m_pRenderTargetView.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
	}

	// DepthStencilView Set
	ComPtr<ID3D11Texture2D> pDSTexture = nullptr;
	D3D11_TEXTURE2D_DESC DescDepth;
	ZeroMemory(&DescDepth, sizeof(D3D11_TEXTURE2D_DESC));
	DescDepth.Width = 1440;
	DescDepth.Height = 900;
	DescDepth.MipLevels = 1;
	DescDepth.ArraySize = 1;
	DescDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
	DescDepth.SampleDesc.Count = 1;
	DescDepth.SampleDesc.Quality = 0;
	DescDepth.CPUAccessFlags = 0;
	DescDepth.MiscFlags = 0;

	DescDepth.Usage = D3D11_USAGE_DEFAULT;
	// ¹é ¹öÆÛ ±íÀÌ ¹× ½ºÅÙ½Ç ¹öÆÛ »ý¼º
	DescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	if (DescDepth.Format == DXGI_FORMAT_R24G8_TYPELESS || DescDepth.Format == DXGI_FORMAT_R32_TYPELESS)
	{
		// ±íÀÌ¸Ê Àü¿ë ±íÀÌ¸Ê »ý¼º
		DescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	}

	hr = DEVICE->CreateTexture2D(&DescDepth, NULL, pDSTexture.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	switch (DescDepth.Format)
	{
	case DXGI_FORMAT_R32_TYPELESS:
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		break;
	case DXGI_FORMAT_R24G8_TYPELESS:
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		break;
	}


	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = DEVICE->CreateDepthStencilView(pDSTexture.Get(), &dsvDesc, m_pDepthStencilView.ReleaseAndGetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
	}

	if (srvDesc.Format == DXGI_FORMAT_R32_FLOAT || srvDesc.Format == DXGI_FORMAT_R24_UNORM_X8_TYPELESS)
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		hr = DEVICE->CreateShaderResourceView(pDSTexture.Get(), &srvDesc, m_pDsvSRV.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}
}

void TestSY::BeginViewPort()
{
	m_iViewPorts = 1;
	// Get Prev ViewPort
	DC->RSGetViewports(&m_iViewPorts, &m_PrevVP);
	DC->OMGetRenderTargets(1, &m_pPrevRTV, &m_pPrevDSV);

	ID3D11ShaderResourceView* pNullSRV = nullptr;
	DC->PSSetShaderResources(0, 1, &pNullSRV);

	ID3D11RenderTargetView* pNullRTV = nullptr;
	DC->OMSetRenderTargets(1, &pNullRTV, NULL);

	DC->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

	// Clear
	const FLOAT color[] = { 1.f, 1.f, 0.f, 1.f };
	DC->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
	DC->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0, 0);
	DC->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_STENCIL, 1.0, 0);

	DC->RSSetViewports(1, &m_vp);
}

void TestSY::EndViewPort()
{
	DC->RSSetViewports(m_iViewPorts, &m_PrevVP);
	DC->OMSetRenderTargets(1, &m_pPrevRTV, m_pPrevDSV);
	m_pPrevRTV->Release(); m_pPrevRTV = nullptr;
	m_pPrevDSV->Release(); m_pPrevDSV = nullptr;
}
