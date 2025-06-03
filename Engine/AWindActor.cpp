#include "pch.h"
#include "AWindActor.h"
#include "CameraManager.h"

Vec3 AWindActor::Prepare(const Vec3& pos, const Vec3& baseVelocity)
{
	Vec3 startPos = GetRandomWindSpawnPos();
	SetPosition(startPos);
	SetRender(true);

	Vec3 dir = GetWindDirection();
	float speed = RandomRange(80.0f, 130.0f);

	return dir * speed;
}

Vec3 AWindActor::GetRandomWindSpawnPos()
{
	Vec3 camPos = CAMERA->Get3DCameraActor()->GetPosition();

	// 화면의 왼쪽 위 근처 범위에서 랜덤 위치
	float xOffset = RandomRange(-500.0f, -200.0f);  // 왼쪽
	float yOffset = RandomRange(200.0f, 400.0f);    // 위쪽

	return camPos + Vec3(xOffset, yOffset, 0.0f);
}

Vec3 AWindActor::GetWindDirection()
{
	float baseAngleRad = atan2(-1.0f, 1.0f); // 오른쪽 아래 방향 기준
	float spreadRad = RandomRange(-15.0f, 15.0f) * 3.141592f / 180.0f;

	float angle = baseAngleRad + spreadRad;
	return Vec3(cos(angle), sin(angle), 0.0f);
}