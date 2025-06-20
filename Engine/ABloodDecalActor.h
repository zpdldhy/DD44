#pragma once
#include "AEffectActor.h"
class ABloodDecalActor : public AEffectActor
{
public:
    ABloodDecalActor() : AEffectActor(EEffectType::BloodDecal) {}

    virtual Vec3 Prepare(const Vec3& pos, const Vec3& baseVelocity, float _scale) override;
};

