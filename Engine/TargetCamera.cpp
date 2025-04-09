#include "pch.h"
#include "TargetCamera.h"

void TargetCamera::UpdateView()
{
	m_CameraData.matView = DirectX::SimpleMath::Matrix::CreateLookAt(m_vEye, m_vTarget, m_vUp);
}
