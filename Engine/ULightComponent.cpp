#include "pch.h"
#include "ULightComponent.h"
#include "Device.h"

void ULightComponent::Init()
{
	USceneComponent::Init(); // 부모 호출 먼저

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(LightData);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = &m_tCBLight;

	DEVICE->CreateBuffer(&bd, &sd, m_pCBLight.GetAddressOf());
}

void ULightComponent::PushLightData()
{
	m_tCBLight.vLightColor_Intensity = Vec4(m_vColor.x, m_vColor.y, m_vColor.z, m_fIntensity);
	m_tCBLight.vLightDirection_Range = Vec4(GetDirection().x, GetDirection().y, GetDirection().z, m_fRange);
	m_tCBLight.vLightPosition_Angle = Vec4(GetLocalPosition().x, GetLocalPosition().y, GetLocalPosition().z, m_fAngle);
	m_tCBLight.vType_AmbientColor = Vec4(static_cast<float>(m_eLightType), m_vAmbientColor.x, m_vAmbientColor.y, m_vAmbientColor.z);
	m_tCBLight.vAmbientPower_Padding = Vec4(m_fAmbientPower, 0.0f, 0.0f, 0.0f);
	DC->UpdateSubresource(m_pCBLight.Get(), 0, nullptr, &m_tCBLight, 0, 0);
	//DC->VSSetConstantBuffers(8, 1, m_pCBLight.GetAddressOf()); // b8
	//DC->PSSetConstantBuffers(8, 1, m_pCBLight.GetAddressOf());
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

LightData ULightComponent::GetLightData(const Vec3& worldPos) const
{
	LightData data = {};

	data.vLightColor_Intensity = Vec4(m_vColor.x, m_vColor.y, m_vColor.z, m_fIntensity);
	data.vLightDirection_Range = Vec4(m_vDirection.x, m_vDirection.y, m_vDirection.z, m_fRange); // ← 수정
	data.vLightPosition_Angle = Vec4(worldPos.x, worldPos.y, worldPos.z, m_fAngle);
	data.vType_AmbientColor = Vec4((float)m_eLightType, m_vAmbientColor.x, m_vAmbientColor.y, m_vAmbientColor.z);
	data.vAmbientPower_Padding = Vec4(m_fAmbientPower, 0.0f, 0.0f, 0.0f);

	return data;
}