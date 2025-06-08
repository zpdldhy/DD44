#pragma once
#include "AEffectActor.h"

class ABeamActor : public AEffectActor
{
public:
    ABeamActor() : AEffectActor(EEffectType::Beam) {}

    virtual Vec3 Prepare(const Vec3& pos, const Vec3& baseVelocity) override;
    virtual void Tick() override;
};
