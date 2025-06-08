#pragma once
#include "AEffectActor.h"
class AFeatherActor : public AEffectActor
{
public:
    AFeatherActor() : AEffectActor(EEffectType::Feather) {}

    virtual Vec3 Prepare(const Vec3& pos, const Vec3& baseVelocity, float _scale) override;
    virtual void Tick() override;
};

