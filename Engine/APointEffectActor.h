#pragma once
#include "AEffectActor.h"
class APointEffectActor : public AEffectActor
{
public:
	APointEffectActor() : AEffectActor(EEffectType::Point) {}

	virtual Vec3 Prepare(const Vec3& pos, const Vec3& baseVelocity, float _scale) override;
};

