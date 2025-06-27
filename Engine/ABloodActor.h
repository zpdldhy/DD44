#pragma once
#include "AEffectActor.h"

class ABloodActor : public AEffectActor
{
public:
    ABloodActor() : AEffectActor(EEffectType::Blood) {}

    virtual Vec3 Prepare(const Vec3& pos, const Vec3& baseVelocity, float _scale) override;
    virtual void Tick();
    void SetBlood(bool _b) { IsState20 = _b; }

private:
    bool IsState20 = false;
};

