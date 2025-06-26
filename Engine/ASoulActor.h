#pragma once
#include "AEffectActor.h"

class ASoulActor : public AEffectActor
{
public:
	ASoulActor() : AEffectActor(EEffectType::Soul) {}

	virtual Vec3 Prepare(const Vec3& pos, const Vec3& baseVelocity, float _scale) override;
	void SetTarget(const Vec3& _targetPos);
	virtual void Tick() override;
	void SetOwnerMonster(const shared_ptr<AActor>& _owner) { m_pOwnerMonster = _owner; }

	enum class SoulState
	{
		IdleWave,
		ArcMove
	};

	SoulState m_eState = SoulState::IdleWave;

	Vec3 m_vArcStart;
	Vec3 m_vArcEnd;
	float m_fArcDuration = 1.5f;
	float m_fArcTimer = 0.f;
private:
	Vec3 m_vTargetPos = Vec3();   
	weak_ptr<AActor> m_pOwnerMonster;
	float m_fSpeed = 5.0f;

	Vec3 m_vStartSoulScale = Vec3(1.5f,1.5f,1.5f);
	Vec3 m_vEndSoulScale = Vec3(.5f, .5f, .5f);
};

