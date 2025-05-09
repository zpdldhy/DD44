#include "pch.h"
#include "ALight.h"

ALight::ALight()
{
    m_pLightComponent = make_shared<ULightComponent>();

    // 슬롯에 등록
    SetLightComponent(m_pLightComponent);

    // Transform도 이 LightComponent로 설정
    m_pTransform = m_pLightComponent;

    // 기본 조명 설정 예시 (원하면 제거 가능)
    m_pLightComponent->SetLightType(ELightType::Directional);
    m_pLightComponent->SetColor(Vec3(1.f, 1.0f, 1.0f));
    m_pLightComponent->SetIntensity(1.0f);
}
