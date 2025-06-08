#pragma once
#include "AActor.h"

class AWindActor : public AActor
{
public:
    AWindActor();
    virtual void Init() override;
    virtual void Tick() override;
    void SetVelocity(const Vec3& _vel) { m_vVelocity = _vel; }

private:
    Vec3 m_vVelocity;
    float m_fRandomOffset = RandomRange(0.f, 3.14f);

};
