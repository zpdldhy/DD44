#include "pch.h"
#include "ASoulActor.h"
#include "Timer.h"
#include "ObjectManager.h"


Vec3 ASoulActor::Prepare(const Vec3& _pos, const Vec3& _baseVelocity, float _scale)
{
	Vec3 pos = _pos;
	SetPosition(pos);
	Vec3 velocity = _baseVelocity;
	velocity.y += 5;
	velocity *= 5;
	SetGravity(0);
	SetStartScale(Vec3(.5f) * _scale);
	SetEndScale(Vec3(.5f) * _scale);
	InitSpriteAnimation(1, 0.6f);
	return velocity;
}

void ASoulActor::SetTarget(const Vec3& _targetPos)
{
	m_vTargetPos = _targetPos;
}


void ASoulActor::Tick()
{
	for (auto& iter : m_vCollisionList)
	{
		auto pActor = OBJECT->GetActor(iter.first);
		if (!pActor) continue;

		m_bRender = false;
		break;
	}
	Vec3 currentPos = GetPosition();
	Vec3 toTarget = m_vTargetPos - currentPos;
	float dist = toTarget.Length();

	toTarget.Normalize();
	float speed = 5.f;
	speed += 1
	m_vVelocity = toTarget * speed;

	AEffectActor::Tick();
}
