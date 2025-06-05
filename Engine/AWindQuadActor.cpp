#include "pch.h"
#include "AWindQuadActor.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"

void AWindQuadActor::Init()
{
    AActor::Init();

    // 풀스크린용 NDC 정규 크기
    SetScale(Vec3(200.f, 200.f, 1.1f));

    // Plane 메시
    auto pMesh = UStaticMeshComponent::CreatePlane();

    // 풀스크린 렌더링용 머티리얼
    m_pMaterial = make_shared<UMaterial>();
    m_pMaterial->Load(L"", L"../Resources/Shader/WindPresent.hlsl");

    pMesh->SetMaterial(m_pMaterial);
    SetMeshComponent(pMesh);
    SetPosition(Vec3(0, 0, 0.97f));
}

void AWindQuadActor::SetSRV(ID3D11ShaderResourceView* _srv)
{
    if (m_pMaterial)
        m_pMaterial->SetTexture(_srv);
}

void AWindQuadActor::Render()
{
    AActor::Render();
}