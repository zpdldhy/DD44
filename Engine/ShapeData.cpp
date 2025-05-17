#include "pch.h"
#include "ShapeData.h"
#include "Input.h"
#include "UCameraComponent.h"
#include "CameraManager.h"

void MouseRay::Click()
{
	WinPos = INPUT->GetMousePos();

	float fWinSizeX = static_cast<float>(g_windowSize.x);
	float fWinSizeY = static_cast<float>(g_windowSize.y);

	auto pCamera = CAMERA->Get3DCameraComponent();

	// To NDC
	EndPos = Vec3(0.f, 0.f, 1.f);
	EndPos.x = (2.f * static_cast<float>(WinPos.x) / fWinSizeX) - 1.f;
	EndPos.y = 1.f - (2.f * static_cast<float>(WinPos.y) / fWinSizeY);

	Matrix mProjViewInvert = Matrix::Identity;
	(pCamera->GetView() * pCamera->GetProjection()).Invert(mProjViewInvert);

	EndPos = Vec3::Transform(EndPos, mProjViewInvert);

	position = pCamera->GetWorldPosition();
	direction = EndPos - position;
}
