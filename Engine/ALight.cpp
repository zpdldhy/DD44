#include "pch.h"
#include "ALight.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"

ALight::ALight()
{
    m_pLightComponent = make_shared<ULightComponent>();

    // 슬롯에 등록
    SetLightComponent(m_pLightComponent);

    // 기본 조명 설정 예시 (원하면 제거 가능)
    m_pLightComponent->SetLightType(ELightType::Directional);
    m_pLightComponent->SetColor(Vec3(1.f, 1.0f, 1.0f));
    m_pLightComponent->SetIntensity(0.5f);

    // CameraComponent 생성
    // Shadow를 만들 때 Texture를 찍기 위함
    auto pCamera = make_shared<UCameraComponent>();
    SetCameraComponent(pCamera);

    auto mesh = UStaticMeshComponent::CreateSphere(10, 10);
    mesh->SetLocalScale(Vec3(1.f, 1.f, 1.f)); // 작게 만들기

    auto material = make_shared<UMaterial>();
    material->Load(L"../Resources/Texture/white.png", L"../Resources/Shader/Default.hlsl");
    mesh->SetMaterial(material);

    SetMeshComponent(mesh); // 부모 클래스에 등록
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