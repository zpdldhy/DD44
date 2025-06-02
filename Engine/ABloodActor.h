#pragma once
#include "AEffectActor.h"
class ABloodActor : public AEffectActor
{
public:
    ABloodActor() : AEffectActor(EEffectType::Blood) {}

    virtual Vec3 Prepare(const Vec3& pos, const Vec3& baseVelocity) override;
};

