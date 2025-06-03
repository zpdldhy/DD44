#pragma once
#include "AEffectActor.h"
class AWindActor : public AEffectActor
{
public:
	AWindActor() : AEffectActor(EEffectType::Wind) {}

	virtual Vec3 Prepare(const Vec3& pos, const Vec3& baseVelocity) override;
	Vec3 GetRandomWindSpawnPos();
	Vec3 GetWindDirection();
};

