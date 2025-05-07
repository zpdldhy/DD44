#include "pch.h"
#include "ALight.h"

ALight::ALight()
{
    m_pLightComponent = make_shared<ULightComponent>();

    // ���Կ� ���
    SetLightComponent(m_pLightComponent);

    // Transform�� �� LightComponent�� ����
    m_pTransform = m_pLightComponent;

    // �⺻ ���� ���� ���� (���ϸ� ���� ����)
    m_pLightComponent->SetLightType(ELightType::Directional);
    m_pLightComponent->SetColor(Vec3(1.f, 0.0f, 0.0f));
    m_pLightComponent->SetIntensity(0.5f);
}
