#pragma once
#include "AEffectActor.h"
class APoppingDust : public AEffectActor
{
public:
    APoppingDust() : AEffectActor(EEffectType::PoppingDust) {}

    virtual Vec3 Prepare(const Vec3& pos, const Vec3& baseVelocity, float scale = 1.f) override;
};

