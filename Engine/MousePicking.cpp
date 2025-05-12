#include "pch.h"
#include "MousePicking.h"
#include "Input.h"
#include "UCameraComponent.h"
#include "CameraManager.h"

void MousePicking::Click()
{
	m_pMouseWinPos = INPUT->GetMousePos();

	float fWinSizeX = static_cast<float>(g_windowSize.x);
	float fWinSizeY = static_cast<float>(g_windowSize.y);

	auto pCamera = CAMERAMANAGER->Get3DCameraComponent();

	// To NDC
	m_vMouseEndPos = Vec3(0.f, 0.f, 1.f);
	m_vMouseEndPos.x = (2.f * static_cast<float>(m_pMouseWinPos.x) / fWinSizeX) - 1.f;
	m_vMouseEndPos.y = 1.f - (2.f * static_cast<float>(m_pMouseWinPos.y) / fWinSizeY);

	Matrix mProjViewInvert = Matrix::Identity;
	(pCamera->GetView() * pCamera->GetProjection()).Invert(mProjViewInvert);

	m_vMouseEndPos = Vec3::Transform(m_vMouseEndPos, mProjViewInvert);

	position = pCamera->GetWorldPosition();
	direction = m_vMouseEndPos - position;
}
