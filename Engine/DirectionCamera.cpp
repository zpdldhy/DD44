#include "pch.h"
#include "DirectionCamera.h"

void DirectionCamera::UpdateView()
{
	m_CameraData.matView = DirectX::SimpleMath::Matrix::CreateLookAt(m_vEye, m_vEye + m_vLook, m_vUp);
}
