#pragma once
#include "AEffectActor.h"
class ADustActor : public AEffectActor
{
public:
    ADustActor() : AEffectActor(EEffectType::Dust) {}

    virtual Vec3 Prepare(const Vec3& pos, const Vec3& baseVelocity) override;
};

