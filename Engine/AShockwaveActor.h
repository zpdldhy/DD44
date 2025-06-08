#pragma once
#include "AEffectActor.h"
class AShockwaveActor : public AEffectActor
{
public:
	AShockwaveActor() : AEffectActor(EEffectType::Shockwave) {}

	virtual Vec3 Prepare(const Vec3& pos, const Vec3& baseVelocity) override;
};

