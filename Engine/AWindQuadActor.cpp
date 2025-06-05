#include "pch.h"
#include "AWindQuadActor.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"

void AWindQuadActor::Init()
{
    AActor::Init();

    // Ǯ��ũ���� NDC ���� ũ��
    SetScale(Vec3(2.0f, 2.0f, 1.0f)); // NDC ���� ��ü ȭ�� ������ ����
    SetPosition(Vec3(0.f, 0.f, 0.97f)); // �߽� ����


    // Plane �޽�
    auto pMesh = UStaticMeshComponent::CreatePlane();

    // Ǯ��ũ�� �������� ��Ƽ����
    m_pMaterial = make_shared<UMaterial>();
    m_pMaterial->Load(L"", L"../Resources/Shader/WindPresent.hlsl");

    pMesh->SetMaterial(m_pMaterial);
    SetMeshComponent(pMesh);
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