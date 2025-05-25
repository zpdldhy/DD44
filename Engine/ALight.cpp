#include "pch.h"
#include "ALight.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"


ALight::ALight()
{
    m_pLightComponent = make_shared<ULightComponent>();

    // ���Կ� ���
    SetLightComponent(m_pLightComponent);
    
    // ���� - ���� �̰�??
    //// Transform�� �� LightComponent�� ����
    //m_pTransform = m_pLightComponent.get();

    // �⺻ ���� ���� ���� (���ϸ� ���� ����)
    m_pLightComponent->SetLightType(ELightType::Directional);
    m_pLightComponent->SetColor(Vec3(1.f, 1.0f, 1.0f));
    m_pLightComponent->SetIntensity(0.5f);

    auto mesh = UStaticMeshComponent::CreateSphere(10, 10);
    mesh->SetLocalScale(Vec3(1.f, 1.f, 1.f)); // �۰� �����

    auto material = make_shared<UMaterial>();
    material->Load(L"../Resources/Texture/white.png", L"../Resources/Shader/Default.hlsl");
    mesh->SetMaterial(material);

    SetMeshComponent(mesh); // �θ� Ŭ������ ���
}
