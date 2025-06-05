#include "pch.h"
#include "AWindActor.h"
#include "UMaterial.h"
#include "Device.h"
#include "Timer.h"
#include "UStaticMeshComponent.h"
#include "USceneComponent.h"

ComPtr<ID3D11Buffer> AWindActor::m_pWindCB = nullptr;

AWindActor::AWindActor()
{
    m_pMaterial = make_shared<UMaterial>();
    m_pMaterial->Load(L"../Resources/Texture/Wind.png", L"../Resources/Shader/Wind.hlsl");
}

void AWindActor::Init()
{
    
    
    AActor::Init();
    CreateWindCB();
    auto mesh = UStaticMeshComponent::CreatePlane();
    mesh->SetMaterial(m_pMaterial);
    SetMeshComponent(mesh);

    SetScale(Vec3(m_vSize.x, m_vSize.y, 1.f));
    SetPosition(Vec3(0.f, 0.f, 0.f));
}

void AWindActor::Tick()
{
    AActor::Tick();

    //m_fTimeAcc += TIMER->GetDeltaTime();

    //m_tWindData.vUVOffset.x = fmod(m_fTimeAcc * m_fSpeed, 1.0f);
  
}

void AWindActor::Render()
{
    if (m_pTransform)
        SetWorldMatrix(m_pTransform->GetWorld());

    if (m_pWindCB)
    {
        // CB 데이터 전달
        DC->UpdateSubresource(m_pWindCB.Get(), 0, nullptr, &m_tWindData, 0, 0);
        DC->VSSetConstantBuffers(12, 1, m_pWindCB.GetAddressOf());
        DC->PSSetConstantBuffers(12, 1, m_pWindCB.GetAddressOf());
    }

    AActor::Render(); // Plane 메시 렌더
}

void AWindActor::CreateWindCB()
{
    if (m_pWindCB) return;

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(WindData);
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA sub = {};
    sub.pSysMem = &m_tWindData;

    HRESULT hr = DEVICE->CreateBuffer(&desc, &sub, m_pWindCB.GetAddressOf());
    DX_CHECK(hr, _T("AWindActor::CreateWindCB"));
}

void AWindActor::SetUV(const Vec2& start, const Vec2& size)
{
    m_tWindData.vUVStart = start;
    m_tWindData.vUVSize = size;
}

void AWindActor::SetSize(const Vec2& size)
{
    m_vSize = size;
    SetScale(Vec3(size.x, size.y, 1.f));
}

void AWindActor::SetSpeed(float speed)
{
    m_fSpeed = speed;
}