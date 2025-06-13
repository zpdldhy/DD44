#include "pch.h"
#include "ALight.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"

ALight::ALight()
{
    m_pLightComponent = make_shared<ULightComponent>();

    // ���Կ� ���
    SetLightComponent(m_pLightComponent);

    // �⺻ ���� ���� ���� (���ϸ� ���� ����)
    m_pLightComponent->SetLightType(ELightType::Directional);
    m_pLightComponent->SetColor(Vec3(1.f, 1.0f, 1.0f));
    m_pLightComponent->SetIntensity(0.5f);

    // CameraComponent ����
    // Shadow�� ���� �� Texture�� ��� ����
    auto pCamera = make_shared<UCameraComponent>();
    SetCameraComponent(pCamera);

    auto mesh = UStaticMeshComponent::CreateSphere(10, 10);
    mesh->SetLocalScale(Vec3(1.f, 1.f, 1.f)); // �۰� �����

    auto material = make_shared<UMaterial>();
    material->Load(L"../Resources/Texture/white.png", L"../Resources/Shader/Default.hlsl");
    mesh->SetMaterial(material);

    SetMeshComponent(mesh); // �θ� Ŭ������ ���
}

void ALight::UpdateLightLookAt(Vec3 _vActorPos, Vec3 _vFocusPos)
{
    SetPosition(_vActorPos);
    GetCameraComponent()->SetLookAt(_vFocusPos);
}

void ALight::UpdateLightLookTo(Vec3 _vActorPos, Vec3 _vFocusDir)
{
    SetPosition(_vActorPos);
    GetCameraComponent()->SetLookTo(_vFocusDir);
}