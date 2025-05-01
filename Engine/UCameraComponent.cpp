#include "pch.h"
#include "UCameraComponent.h"

void UCameraComponent::Init()
{

}

void UCameraComponent::Tick()
{
	UpdateView();

	if (m_ProjectionType == ProjectionType::PT_ORTHOGRAPHIC)
		UpdateOrthographicProjection();
	else if (m_ProjectionType == ProjectionType::PT_PERSPECTIVE)
		UpdatePersPectiveProjection();
}

void UCameraComponent::Render()
{
}

void UCameraComponent::Destroy()
{
}

void UCameraComponent::UpdateView()
{
	m_vEye = m_pOwner->m_vPosition + m_vLocalPosition;	

	if (Vec3::Distance(Vec3(0.f, 0.f, 0.f), m_vLocalPosition) < 0.1f)
		m_vLook = m_pOwner->m_vLook;
	else
		m_vLook = -m_vLocalPosition;

	Vec3 vUp = Vec3(0.f, 1.f, 0.f);

	m_matView = DirectX::XMMatrixLookAtLH(m_vEye, m_vEye + m_vLook, vUp);
}

void UCameraComponent::UpdateOrthographicProjection()
{	
	m_matProjection = DirectX::XMMatrixOrthographicLH(m_fWidth, m_fHeight, m_fNear, m_fFar);
}

void UCameraComponent::UpdatePersPectiveProjection()
{
	m_matProjection = DirectX::XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNear, m_fFar);
}

void UCameraComponent::SetOrthographic(float _fWidth, float _fHeight)
{
	m_fWidth = _fWidth;
	m_fHeight = _fHeight;
}

void UCameraComponent::SetPerspective(float _fFov, float _fAspect, float _fNear, float _fFar)
{
	m_fFov = _fFov;
	m_fAspect = _fAspect;
	m_fNear = _fNear;
	m_fFar = _fFar;
}