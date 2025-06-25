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

private:
	Vec3 m_vTargetPos = Vec3();   
	weak_ptr<AActor> m_pOwnerMonster;
};

