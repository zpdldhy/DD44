#include "pch.h"
#include "ULightComponent.h"
#include "Device.h"

void ULightComponent::Init()
{
	USceneComponent::Init(); // 부모 호출 먼저

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CB_LIGHT);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = &m_tCBLight;

	DEVICE->CreateBuffer(&bd, &sd, m_pCBLight.GetAddressOf());
}

void ULightComponent::PushLightData()
{
	m_tCBLight.vLightColor = m_vColor;
	m_tCBLight.fIntensity = m_fIntensity;
	m_tCBLight.vLightDirection = { 1.0f , 0.0f , 0.0f };//GetLook();      // Z축 forward
	m_tCBLight.fRange = m_fRange;
	m_tCBLight.vLightPosition = GetPosition();
	m_tCBLight.fAngle = m_fAngle;
	m_tCBLight.iLightType = static_cast<int>(m_eLightType);
	m_tCBLight.vAmbientColor = m_vAmbientColor;
	m_tCBLight.fAmbientPower = m_fAmbientPower;
	DC->UpdateSubresource(m_pCBLight.Get(), 0, nullptr, &m_tCBLight, 0, 0);
	DC->VSSetConstantBuffers(8, 1, m_pCBLight.GetAddressOf()); // b8
	DC->PSSetConstantBuffers(8, 1, m_pCBLight.GetAddressOf());
}

void ULightComponent::Render()
{
	PushLightData();
}

ULightComponent::ULightComponent()
{
	// 기본값은 Directional Light
	m_vColor = Vec3(1.0f, 1.0f, 1.0f);
	m_fIntensity = 1.0f;
	m_fRange = 10.0f;
	m_fAngle = 45.0f;
}