#include "pch.h"
#include "TempBossCamera.h"

void TempBossCamera::Init()
{
	Vec3 tempLook = {0, 0, -1};
	GetOwner()->GetCameraComponent()->SetLookTo(tempLook);
}

void TempBossCamera::Tick()
{
}
